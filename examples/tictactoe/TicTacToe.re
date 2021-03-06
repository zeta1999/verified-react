open Css;

type logic_state = {
  game: TicTacToe_Logic.game_state,
  status: TicTacToe_Logic.game_status,
};

type state = {
  customInitialLogicState: option(logic_state),
  logic: logic_state,
};

type action =
  | Move(TicTacToe_Logic.move)
  | Restart;

/* Component template declaration.
   Needs to be **after** state and action declarations! */
let component = ReasonReact.reducerComponent("Example");

let defaultInitialState = {
  game: TicTacToe_Logic.initial_game,
  status: TicTacToe_Logic.status(TicTacToe_Logic.initial_game),
};

/*graphic symbols for the game*/
let svgE = <img src=[%raw "require('../../img/sign-e.svg')"] />;
let svgX = <img src=[%raw "require('../../img/sign-x.svg')"] />;
let svgO = <img src=[%raw "require('../../img/sign-o.svg')"] />;

/* greeting and children are props. `children` isn't used, therefore ignored.
   We ignore it by prepending it with an underscore */
let make = (~customInitialLogicState, ~onGameFinished, _children) => {
  /* spread the other default fields of component here and override a few */
  ...component,
  initialState: () =>
    switch (customInitialLogicState) {
    | None => {logic: defaultInitialState, customInitialLogicState: None}
    | Some(il) => {logic: il, customInitialLogicState: Some(il)}
    },
  willReceiveProps: self =>
    switch (customInitialLogicState, self.state.customInitialLogicState) {
    | (Some(l1) as a, b) when a != b => {
        logic: l1,
        customInitialLogicState: Some(l1),
      }
    | _ => self.state
    },
  /* State transitions */
  reducer: (action, state) =>
    switch (action) {
    | Restart =>
      ReasonReact.Update({
        ...state,
        logic:
          switch (customInitialLogicState) {
          | None => defaultInitialState
          | Some(s) => s
          },
      })
    | Move(move) =>
      let (next, status) = TicTacToe_Logic.play(state.logic.game, move);
      let newState = {
        ...state,
        logic: {
          status,
          game: next,
        },
      };
      switch (status) {
      | Won(_)
      | Tied =>
        ReasonReact.UpdateWithSideEffects(
          newState,
          (_self => onGameFinished()),
        )
      | InProgress
      | InvalidMove(_) => ReasonReact.Update(newState)
      };
    },
  render: self => {
    let label = s =>
      switch (s) {
      | None => <div />
      | Some(TicTacToe_Logic.X) => svgX
      | Some(TicTacToe_Logic.O) => svgO
      };
    let disabled =
      switch (self.state.logic.status) {
      | InProgress => false
      | InvalidMove(_) => false
      | _ => true
      };
    let buttonCss = b => {
      let base =
        style([
          display(block),
          width(px(88)),
          height(px(88)),
          margin(px(3)),
        ]);
      switch (self.state.logic.status) {
      | InvalidMove(m) when b == m =>
        merge([base, style([border(px(1), solid, red)])])
      | _ => base
      };
    };
    let overlay =
      switch (self.state.logic.status) {
      | Tied => Some(svgE)
      | Won(X) => Some(svgX)
      | Won(O) => Some(svgO)
      | _ => None
      };
    let rowCss = style([display(flexBox), flexDirection(row)]);
    let elems =
      <div className="tictactoe__instances">
        <div className=rowCss>
          <button
            className={"tictactoe__button " ++ buttonCss(A)}
            onClick={_event => self.send(Move(A))}
            disabled>
            {label(self.state.logic.game.grid.a)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(B)}
            onClick={_event => self.send(Move(B))}
            disabled>
            {label(self.state.logic.game.grid.b)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(C)}
            onClick={_event => self.send(Move(C))}
            disabled>
            {label(self.state.logic.game.grid.c)}
          </button>
        </div>
        <div className=rowCss>
          <button
            className={"tictactoe__button " ++ buttonCss(D)}
            onClick={_event => self.send(Move(D))}
            disabled>
            {label(self.state.logic.game.grid.d)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(E)}
            onClick={_event => self.send(Move(E))}
            disabled>
            {label(self.state.logic.game.grid.e)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(F)}
            onClick={_event => self.send(Move(F))}
            disabled>
            {label(self.state.logic.game.grid.f)}
          </button>
        </div>
        <div className=rowCss>
          <button
            className={"tictactoe__button " ++ buttonCss(G)}
            onClick={_event => self.send(Move(G))}
            disabled>
            {label(self.state.logic.game.grid.g)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(H)}
            onClick={_event => self.send(Move(H))}
            disabled>
            {label(self.state.logic.game.grid.h)}
          </button>
          <button
            className={"tictactoe__button " ++ buttonCss(I)}
            onClick={_event => self.send(Move(I))}
            disabled>
            {label(self.state.logic.game.grid.i)}
          </button>
        </div>
      </div>;
    let sub =
      switch (overlay) {
      | None => [|elems|]
      | Some(overlayText) => [|
          elems,
          <div
            className={
              style([
                position(absolute),
                top(px(0)),
                left(px(0)),
                width(pct(100.)),
                height(pct(100.)),
                textAlign(center),
                fontSize(px(150)),
                display(flexBox),
                flexDirection(row),
                justifyContent(spaceAround),
                userSelect(none),
                cursor(`pointer),
              ])
            }
            onClick=(_event => self.send(Restart))>
            <div
              className={
                "tictactoe__result "
                ++ style([
                     display(flexBox),
                     flexDirection(column),
                     justifyContent(spaceAround),
                   ])
              }>
              overlayText
            </div>
          </div>,
        |]
      };
    <div>
      <div
        className={
          style([
            position(relative),
            display(flexBox),
            justifyContent(spaceAround),
          ])
        }>
        ...sub
      </div>
      <div
        className={
          style([
            display(flexBox),
            flexDirection(column),
            alignItems(center),
            marginLeft(px(10)),
          ])
        }>
        <div className={style([marginBottom(px(10))])}>
          {ReasonReact.string("Next player:")}
        </div>
        <div className="tictactoe__button tictactoe__button--silent ">
          {
            let next_player =
              switch (self.state.logic.game.last_player) {
              | None => TicTacToe_Logic.initial_player
              | Some(p) => TicTacToe_Logic.other_player(p)
              };
            switch (next_player) {
            | X => svgX
            | O => svgO
            };
          }
        </div>
      </div>
    </div>;
  },
};