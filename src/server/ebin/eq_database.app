{application, eq_database,
[
  {description, "eq_database"},
  {vsn, "1.0"},
  {id, "eq_database"},
  %%{modules,      [tcp_listener, tcp_mm]},
  %%{registered,   [tcp_server_sup, tcp_listener]},
  {applications, [kernel, stdlib]},
  %%
  %% mod: Specify the module name to start the application, plus args
  %%
  {mod, {database, []}}
]
}.

