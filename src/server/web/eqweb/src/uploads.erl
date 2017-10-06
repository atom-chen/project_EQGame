%% @author Joe Freeman <joe.freeman@bitroot.com>
%% @copyright 2009 Joe Freeman

%% @doc This is part of the 'Handling multipart uploads with MochiWeb' demo:
%% http://joefreeman.co.uk/blog/2009/12/handling-multipart-uploads-with-mochiweb
%%
%% Available under http://en.wikipedia.org/wiki/MIT_License

-module(uploads).
-author('Joe Freeman <joe.freeman@bitroot.com>').

-export([do/2]).

%% External API

%% @doc This is the client-handling loop that MochiWeb will use for each
%% client that connects to the web server.
do(Req, DocRoot) ->
    PhotoDir = DocRoot ++ "/upload/",
    %% Upload a photo
    ValidExtensions = [".txt", ".log", ".dmp"],
    upload_photo(Req, PhotoDir, ValidExtensions).

%% Internal API

%% @doc Handle uploading of a photo. Send a redirect response on success, or
%% inform the user of the error. The uploaded photo, if valid, will be moved
%% into the photos directory.
upload_photo(Req, PhotoDir, ValidExtensions) ->
    % Setup the file handler and parse the multipart data
    FileHandler = fun(Filename, ContentType) -> handle_file(Filename, ContentType) end,
    Files = mochiweb_multipart:parse_form(Req, FileHandler),
    % Get the details of our 'photo'
    {OriginalFilename, _, TempFilename} = proplists:get_value("file", Files),
    % Check the file extension is valid
    case lists:member(filename:extension(OriginalFilename), ValidExtensions) of
        true ->
            % Attempt to move the file into the photos directory
	    FileNameArr = string:tokens(OriginalFilename, "__"),
	    NFileName = lists:nth(1, FileNameArr) ++ "/" ++ lists:nth(2, FileNameArr) ++ "/" ++ lists:nth(3, FileNameArr),
	    file:make_dir(PhotoDir ++ lists:nth(1, FileNameArr)),
	    file:make_dir(PhotoDir ++ lists:nth(1, FileNameArr) ++ "/" ++ lists:nth(2, FileNameArr)),
            Destination = PhotoDir ++ NFileName,
            case file:copy(TempFilename, Destination) of
                {ok, _} ->
		    file:delete(TempFilename),
                    % Success, redirect the user back to the gallery
                    Url = "/",
                    Req:respond({302, [{"Location", Url}], "Redirecting to " ++ Url});
                {error, Reason} ->
                    % Something went wrong
                    file:delete(TempFilename),
                    html_response(Req, "An error occured whilst trying to move your file: " ++ atom_to_list(Reason) ++ ". Does the destination directory exist?")
            end;
        false ->
            % User tried to upload a file with an invalid extension
            file:delete(TempFilename),
            html_response(Req, "Invalid file type. File extension must be one of: " ++ string:join(ValidExtensions, ", ") ++ ". <a href=\"/\">Try again?</a>")
    end.

%% @doc Send an HTML response back to the browser.
html_response(Req, Response) ->
    Req:ok({"text/html", [], Response}).

%% @doc Handle a file. A 'chunk' handling function will be returned.
handle_file(Filename, ContentType) ->
    TempFilename = "/tmp/" ++ atom_to_list(?MODULE) ++ integer_to_list(erlang:phash2(make_ref())),
    {ok, File} = file:open(TempFilename, [raw, write]),
    chunk_handler(Filename, ContentType, TempFilename, File).

%% @doc Return a function for handling chunks of data. If the 'eof' atom is
%% passed to the returned function then the file will be closed and details
%% returned. Otherwise, a function will be returned which will be able to
%% handle the next chunk of data.
chunk_handler(Filename, ContentType, TempFilename, File) ->
    fun(Next) ->
        case Next of
            
            eof ->
                % End of part: close file and return details of the upload
                file:close(File),
                {Filename, ContentType, TempFilename};
                
            Data ->
                % More data to write to the file
                file:write(File, Data),
                chunk_handler(Filename, ContentType, TempFilename, File)
        end
    end.
