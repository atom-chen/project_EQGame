{application, tcp_farm,
[
  {description, "farm server"},
  {vsn, "1.0"},
  {id, "tcp_farm"},
  {modules,      [tcp_listener, tcp_farm]},
  {registered,   [tcp_server_sup, tcp_listener]},
  {applications, [kernel, stdlib]},
  %%
  %% mod: Specify the module name to start the application, plus args
  %%
  {mod, {tcp_server_app, []}},
  {env, [{template_data, "./template/"}, {listen_port, 2223}, {callback, tcp_farm}]}
]
}.

