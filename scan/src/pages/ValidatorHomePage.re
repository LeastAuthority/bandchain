module Styles = {
  open Css;

  let vFlex = style([display(`flex), flexDirection(`row), alignItems(`center)]);

  let pageContainer = style([paddingTop(`px(35))]);
  let validatorsLogo = style([marginRight(`px(10))]);
  let highlight = style([margin2(~v=`px(28), ~h=`zero)]);
  let valueContainer = style([display(`flex), justifyContent(`flexStart)]);
  let monikerContainer = style([maxWidth(`px(180))]);

  let emptyContainer =
    style([
      display(`flex),
      justifyContent(`center),
      alignItems(`center),
      height(`px(300)),
      boxShadow(Shadow.box(~x=`px(0), ~y=`px(2), ~blur=`px(2), Css.rgba(0, 0, 0, 0.05))),
      backgroundColor(white),
      marginBottom(`px(1)),
    ]);

  let seperatedLine =
    style([
      width(`px(13)),
      height(`px(1)),
      marginLeft(`px(10)),
      marginRight(`px(10)),
      backgroundColor(Colors.mediumGray),
    ]);

  let fullWidth =
    style([
      width(`percent(100.0)),
      display(`flex),
      paddingLeft(`px(26)),
      paddingRight(`px(46)),
    ]);

  let icon =
    style([
      width(`px(30)),
      height(`px(30)),
      marginTop(`px(5)),
      marginLeft(Spacing.xl),
      marginRight(Spacing.xl),
    ]);
};

module ToggleButton = {
  open Css;

  [@react.component]
  let make = (~isActive, ~setIsActive) => {
    <div className={style([display(`flex), alignItems(`center)])}>
      <div
        onClick={_ => setIsActive(_ => true)}
        className={style([display(`flex), cursor(`pointer)])}>
        <Text value="Active" color=Colors.darkPurple />
      </div>
      <HSpacing size=Spacing.sm />
      <div
        className={style([
          display(`flex),
          justifyContent(isActive ? `flexStart : `flexEnd),
          backgroundColor(Colors.fadePurple),
          borderRadius(`px(15)),
          padding2(~v=`px(2), ~h=`px(3)),
          width(`px(45)),
          cursor(`pointer),
          boxShadow(
            Shadow.box(
              ~inset=true,
              ~x=`zero,
              ~y=`zero,
              ~blur=`px(4),
              isActive ? Colors.borderPurple : Colors.mediumGray,
            ),
          ),
        ])}
        onClick={_ => setIsActive(oldVal => !oldVal)}>
        <img
          src={isActive ? Images.activeValidatorLogo : Images.inactiveValidatorLogo}
          className={style([width(`px(15))])}
        />
      </div>
      <HSpacing size=Spacing.sm />
      <div
        onClick={_ => setIsActive(_ => false)}
        className={style([display(`flex), cursor(`pointer)])}>
        <Text value="Inactive" />
      </div>
    </div>;
  };
};

let renderBody = (idx: int, validator: ValidatorHook.Validator.t) => {
  let votingPower = validator.votingPower;
  let token = validator.tokens;
  let commission = validator.commission;
  let uptime = validator.uptime;
  let allRequestCount =
    validator.completedRequestCount + validator.missedRequestCount |> float_of_int;
  let reportRate = (validator.completedRequestCount |> float_of_int) /. allRequestCount *. 100.;

  <TBody key={idx |> string_of_int}>
    <div className=Styles.fullWidth>
      <Row>
        <Col size=0.8 alignSelf=Col.Start>
          <Col size=1.6 alignSelf=Col.Start>
            <Text
              value={idx + 1 |> string_of_int}
              color=Colors.mediumGray
              code=true
              weight=Text.Regular
              spacing={Text.Em(0.02)}
              block=true
              size=Text.Md
            />
          </Col>
        </Col>
        <Col size=1.9 alignSelf=Col.Start>
          <div className=Styles.monikerContainer> <ValidatorMonikerLink validator /> </div>
        </Col>
        <Col size=1.4 alignSelf=Col.Start>
          <div>
            <Text
              value={token |> Format.fPretty}
              color=Colors.mediumGray
              code=true
              weight=Text.Regular
              spacing={Text.Em(0.02)}
              block=true
              align=Text.Right
              size=Text.Md
            />
            <VSpacing size=Spacing.sm />
            <Text
              value={"(" ++ votingPower->Js.Float.toFixedWithPrecision(~digits=2) ++ "%)"}
              color=Colors.mediumLightGray
              code=true
              weight=Text.Thin
              spacing={Text.Em(0.02)}
              block=true
              align=Text.Right
              size=Text.Md
            />
          </div>
        </Col>
        <Col size=1.2 alignSelf=Col.Start>
          <Text
            value={commission->Js.Float.toFixedWithPrecision(~digits=2)}
            color=Colors.mediumGray
            code=true
            weight=Text.Regular
            spacing={Text.Em(0.02)}
            block=true
            align=Text.Right
            size=Text.Md
          />
        </Col>
        <Col size=1.1 alignSelf=Col.Start>
          <Text
            value={uptime->Js.Float.toFixedWithPrecision(~digits=2)}
            color=Colors.mediumGray
            code=true
            weight=Text.Regular
            spacing={Text.Em(0.02)}
            block=true
            align=Text.Right
            size=Text.Md
          />
        </Col>
        <Col size=1.2 alignSelf=Col.Start>
          <Text
            value={reportRate->Js.Float.toFixedWithPrecision(~digits=2)}
            color=Colors.mediumGray
            code=true
            weight=Text.Regular
            spacing={Text.Em(0.02)}
            block=true
            align=Text.Right
            size=Text.Md
          />
        </Col>
      </Row>
    </div>
  </TBody>;
};

[@react.component]
let make = () => {
  let (isActive, setIsActive) = React.useState(_ => true);
  let validatorsOpt =
    ValidatorHook.getList(~status=isActive ? ValidatorHook.Bonded : ValidatorHook.Unbonded, ());
  let globalInfo = ValidatorHook.getGlobalInfo();
  let bondedValidatorCount = ValidatorHook.getValidatorCount(~status=ValidatorHook.Bonded, ());
  let unbondedValidatorCount =
    ValidatorHook.getValidatorCount(~status=ValidatorHook.Unbonded, ());
  let unbondingValidatorCount =
    ValidatorHook.getValidatorCount(~status=ValidatorHook.Unbonding, ());
  let allValidatorCount = bondedValidatorCount + unbondedValidatorCount + unbondingValidatorCount;

  let allBondedAmountOpt = {
    let%Opt validators = validatorsOpt;

    Some(
      validators->Belt.List.reduce(0., (acc, validator) => acc +. validator.tokens)
      |> int_of_float,
    );
  };

  <div className=Styles.pageContainer>
    <Row justify=Row.Between>
      <Col>
        <div className=Styles.vFlex>
          <img src=Images.validators className=Styles.validatorsLogo />
          <Text
            value="ALL VALIDATORS"
            weight=Text.Medium
            size=Text.Md
            nowrap=true
            color=Colors.mediumGray
            spacing={Text.Em(0.06)}
          />
          <div className=Styles.seperatedLine />
          <Text value={(allValidatorCount |> string_of_int) ++ " In total"} />
        </div>
      </Col>
      <Col> <ToggleButton isActive setIsActive /> </Col>
    </Row>
    <div className=Styles.highlight>
      <Row>
        <Col size=0.7>
          {switch (validatorsOpt) {
           | Some(validators) =>
             <InfoHL
               info={InfoHL.Fraction(validators->Belt.List.length, allValidatorCount, false)}
               header="VALIDATORS"
             />
           | None => <InfoHL info={InfoHL.Text("?")} header="VALIDATORS" />
           }}
        </Col>
        <Col size=1.1>
          {switch (allBondedAmountOpt) {
           | Some(allBondedAmount) =>
             <InfoHL
               info={InfoHL.Fraction(allBondedAmount, globalInfo.totalSupply, true)}
               header="BONDED TOKENS"
             />
           | None => <InfoHL info={InfoHL.Text("?")} header="BONDED TOKENS" />
           }}
        </Col>
        <Col size=0.9>
          <InfoHL
            info={InfoHL.FloatWithSuffix(globalInfo.inflationRate, "  %")}
            header="INFLATION RATE"
          />
        </Col>
        <Col size=0.51>
          <InfoHL
            info={InfoHL.FloatWithSuffix(globalInfo.avgBlockTime, "  secs")}
            header="24 HOUR AVG BLOCK TIME"
          />
        </Col>
      </Row>
    </div>
    // TODO : Add toggle button
    <THead>
      <div className=Styles.fullWidth>
        <Row>
          {[
             ("RANK", 0.8),
             ("VALIDATOR", 1.9),
             ("VOTING POWER (BAND)", 1.4),
             ("COMMISSION (%)", 1.2),
             ("UPTIME (%)", 1.1),
             ("REPORT RATE (%)", 1.2),
           ]
           ->Belt.List.mapWithIndex((idx, (title, size)) => {
               <Col size key=title>
                 <Text
                   block=true
                   value=title
                   size=Text.Sm
                   weight=Text.Semibold
                   align=?{idx > 1 ? Some(Text.Right) : None}
                   color=Colors.mediumLightGray
                   spacing={Text.Em(0.1)}
                 />
               </Col>
             })
           ->Array.of_list
           ->React.array}
        </Row>
      </div>
    </THead>
    {switch (validatorsOpt) {
     | Some(validators) =>
       if (validators->Belt_List.length > 0) {
         validators
         ->Belt.List.toArray
         ->Belt_Array.mapWithIndex((idx, validator) => renderBody(idx, validator))
         ->React.array;
       } else {
         <div className=Styles.emptyContainer> <Text value="No Validators" size=Text.Xxl /> </div>;
       }
     | None =>
       <div className=Styles.emptyContainer> <Text value="Loading..." size=Text.Xxl /> </div>
     }}
    <VSpacing size=Spacing.lg />
  </div>;
};