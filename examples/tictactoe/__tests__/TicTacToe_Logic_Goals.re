open Jest;

let serverInfo = ref(None);

let moduleName = "TicTacToe_Logic";

module I = Imandra_client;

let () =
  beforeAllPromise(() => {
    let si = Imandra_client.Server_info.from_file() |> Belt.Result.getExn;
    serverInfo := Some(si);
    Js.Promise.resolve();
  });

let syntax = I.Api.Reason;

let ppErr = e => Format.asprintf("%a", I.Error.pp, e);

let () =
  describe("tic tac toe model", () => {
    beforeAllPromise(() => {
      let ip = serverInfo^ |> Belt.Option.getExn;
      let model_path =
        Node.Path.join([|
          [%raw "__dirname"],
          "..",
          Printf.sprintf("%s.ire", moduleName),
        |]);
      Imandra_client.Eval.by_src(
        ip,
        ~syntax,
        ~src=Printf.sprintf("#mod_use \"%s\"", model_path),
      )
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok(_) => Js.Promise.resolve(pass)
           | Belt.Result.Error(e) => {
               Js.Console.error(ppErr(e));
               Js.Promise.reject(Failure(ppErr(e)));
             },
         );
    });

    afterAllPromise(() =>
      Js.Promise.make((~resolve, ~reject) =>
        Js.Global.setTimeout(
          () =>
            Imandra_client.reset(serverInfo^ |> Belt.Option.getExn)
            |> Js.Promise.then_(
                 fun
                 | Belt.Result.Ok(_) => {
                     resolve(. pass);
                     Js.Promise.resolve();
                   }
                 | Belt.Result.Error(e) => {
                     Js.Console.error(I.Error.pp_str(e));
                     reject(. Failure(I.Error.pp_str(e)));
                     Js.Promise.resolve();
                   },
               )
            |> ignore,
          500,
        )
        |> ignore
      )
    );

    testPromise("verify move difference", () => {
      let ip = serverInfo^ |> Belt.Option.getExn;
      let functionName =
        Imandra_client.function_name(
          TicTacToe_Logic.goal_valid_grid_has_no_more_than_one_move_diff,
        );
      let name = Printf.sprintf("%s.%s", moduleName, functionName);
      Imandra_client.Verify.by_name(ip, ~name)
      |> Js.Promise.then_(
           fun
           | Belt.Result.Ok(I.Api.Response.V_proved) => {
               Js.Promise.resolve(pass);
             }
           | Belt.Result.Ok(o) => {
               Js.Console.error(o);
               Js.Promise.reject(Failure("unexpected result"));
             }
           | Belt.Result.Error(e) => {
               Js.Console.error(ppErr(e));
               Js.Promise.reject(Failure(ppErr(e)));
             },
         );
    });
    testPromise(
      ~timeout=300000,
      "verify game progression",
      () => {
        let ip = serverInfo^ |> Belt.Option.getExn;
        let functionName =
          Imandra_client.function_name(TicTacToe_Logic.goal_game_progression);
        let name = Printf.sprintf("%s.%s", moduleName, functionName);
        Imandra_client.Verify.by_name(ip, ~name)
        |> Js.Promise.then_(
             fun
             | Belt.Result.Ok(I.Api.Response.V_proved) =>
               Js.Promise.resolve(pass)
             | Belt.Result.Ok(o) => {
                 Js.Console.error(o);
                 Js.Promise.reject(Failure("unexpected result"));
               }
             | Belt.Result.Error(e) => {
                 Js.Console.error(ppErr(e));
                 Js.Promise.reject(Failure(ppErr(e)));
               },
           );
      },
    );
  });
