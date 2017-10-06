{application, gamesvr,
[
  {description, "game server"},
  {vsn, "1.0"},
  {id, "gamesvr"},
  {modules,      []},
  {registered,   []},
  {applications, [kernel, stdlib]},
  %%
  %% mod: Specify the module name to start the application, plus args
  %%
  {mod, {gamesvr_app, []}},
  {env, [{template_data, "./template/"}]}
]
}.

