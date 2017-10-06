{application, farmsvr,
[
  {description, "farm server"},
  {vsn, "1.0"},
  {id, "farmsvr"},
  {modules,      []},
  {registered,   []},
  {applications, [kernel, stdlib]},
  %%
  %% mod: Specify the module name to start the application, plus args
  %%
  {mod, {farmsvr_app, []}},
  {env, [{template_data, "./template/"}]}
]
}.

