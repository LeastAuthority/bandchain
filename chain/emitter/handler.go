package emitter

import (
	sdk "github.com/cosmos/cosmos-sdk/types"
	"github.com/cosmos/cosmos-sdk/x/bank"
	dist "github.com/cosmos/cosmos-sdk/x/distribution"
	"github.com/cosmos/cosmos-sdk/x/slashing"
	"github.com/cosmos/cosmos-sdk/x/staking"
	abci "github.com/tendermint/tendermint/abci/types"

	"github.com/bandprotocol/bandchain/chain/x/oracle"
	"github.com/bandprotocol/bandchain/chain/x/oracle/types"
)

func parseEvents(events sdk.StringEvents) EvMap {
	evMap := make(EvMap)
	for _, event := range events {
		for _, kv := range event.Attributes {
			key := event.Type + "." + kv.Key
			evMap[key] = append(evMap[key], kv.Value)
		}
	}
	return evMap
}

// handleMsg handles the given message by publishing relevant events and populates accounts
// that need balance update in 'app.accs'. Also fills in extra info for this message.
func (app *App) handleMsg(txHash []byte, msg sdk.Msg, log sdk.ABCIMessageLog, extra JsDict) {
	evMap := parseEvents(log.Events)
	switch msg := msg.(type) {
	case oracle.MsgRequestData:
		app.handleMsgRequestData(txHash, msg, evMap, extra)
	case oracle.MsgReportData:
		app.handleMsgReportData(txHash, msg, evMap, extra)
	case oracle.MsgCreateDataSource:
		app.handleMsgCreateDataSource(txHash, msg, evMap, extra)
	case oracle.MsgCreateOracleScript:
		app.handleMsgCreateOracleScript(txHash, msg, evMap, extra)
	case oracle.MsgEditDataSource:
		app.handleMsgEditDataSource(txHash, msg, evMap, extra)
	case oracle.MsgEditOracleScript:
		app.handleMsgEditOracleScript(txHash, msg, evMap, extra)
	case staking.MsgCreateValidator:
		app.handleMsgCreateValidator(msg)
	case staking.MsgEditValidator:
		app.handleMsgEditValidator(msg)
	case staking.MsgDelegate:
		app.handleMsgDelegate(msg)
	case staking.MsgUndelegate:
		app.handleMsgUndelegate(msg)
	case staking.MsgBeginRedelegate:
		app.handleMsgBeginRedelegate(msg)
	case bank.MsgSend:
		app.handleMsgSend(msg)
	case bank.MsgMultiSend:
		app.handleMsgMultiSend(msg)
	case dist.MsgWithdrawDelegatorReward:
		app.handleMsgWithdrawDelegatorReward(txHash, msg, evMap, extra)
	case slashing.MsgUnjail:
		app.handleMsgUnjail(msg)
	}
}

func (app *App) handleBeginBlockEndBlockEvent(event abci.Event) {
	events := sdk.StringifyEvents([]abci.Event{event})
	evMap := parseEvents(events)
	switch event.Type {
	case types.EventTypeResolve:
		app.handleEventRequestExecute(evMap)
	case slashing.EventTypeSlash:
		app.handleEventSlash(evMap)
	default:
		break
	}
}
