{application, eqweb,
 [{description, "eqweb"},
  {vsn, "0.01"},
  {modules, [
    eqweb,
    eqweb_app,
    eqweb_sup,
    eqweb_web,
    eqweb_deps
  ]},
  {registered, []},
  {mod, {eqweb_app, []}},
  {env, []},
  {applications, [kernel, stdlib, crypto]}]}.
