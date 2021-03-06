package main

import (
	"fmt"

	sdkCtx "github.com/cosmos/cosmos-sdk/client/context"
	ckeys "github.com/cosmos/cosmos-sdk/client/keys"
	sdk "github.com/cosmos/cosmos-sdk/types"
	"github.com/cosmos/cosmos-sdk/x/auth"

	"github.com/bandprotocol/bandchain/chain/app"
	otypes "github.com/bandprotocol/bandchain/chain/x/oracle/types"
	rpcclient "github.com/tendermint/tendermint/rpc/client"
)

var (
	cdc = app.MakeCodec()
)

func SubmitReport(c *Context, l *Logger, id otypes.RequestID, reps []otypes.RawReport) {
	key := <-c.keys
	defer func() {
		c.keys <- key
	}()

	msg := otypes.NewMsgReportData(otypes.RequestID(id), reps, c.validator, key.GetAddress())
	if err := msg.ValidateBasic(); err != nil {
		l.Error(":exploding_head: Failed to validate basic with error: %s", err.Error())
		return
	}

	cliCtx := sdkCtx.CLIContext{Client: c.client}
	acc, err := auth.NewAccountRetriever(cliCtx).GetAccount(key.GetAddress())
	if err != nil {
		l.Error(":exploding_head: Failed to retreive account with error: %s", err.Error())
		return
	}

	txBldr := auth.NewTxBuilder(
		auth.DefaultTxEncoder(cdc), acc.GetAccountNumber(), acc.GetSequence(),
		200000, 1, false, cfg.ChainID, "", sdk.NewCoins(), c.gasPrices,
	)
	// txBldr, err = authclient.EnrichWithGas(txBldr, cliCtx, []sdk.Msg{msg})
	// if err != nil {
	// 	l.Error(":exploding_head: Failed to enrich with gas with error: %s", err.Error())
	// 	return
	// }
	out, err := txBldr.WithKeybase(keybase).BuildAndSign(key.GetName(), ckeys.DefaultKeyPass, []sdk.Msg{msg})
	if err != nil {
		l.Error(":exploding_head: Failed to build tx with error: %s", err.Error())
		return
	}

	res, err := cliCtx.BroadcastTxCommit(out)
	if err != nil {
		l.Error(":exploding_head: Failed to broadcast tx with error: %s", err.Error())
		return
	}
	if res.Code != 0 {
		l.Error(":exploding_head: Tx returned nonzero code %d with log %s, tx hash: %s", res.Code, res.RawLog, res.TxHash)
		return
	}
	l.Info(":smiling_face_with_sunglasses: Successfully broadcast tx with hash: %s", res.TxHash)
}

// GetExecutable fetches data source executable using the provided client.
func GetExecutable(c *Context, l *Logger, hash string) ([]byte, error) {
	resValue, err := c.fileCache.GetFile(hash)
	if err != nil {
		l.Debug(":magnifying_glass_tilted_left: Fetching data source hash: %s from bandchain querier", hash)
		res, err := c.client.ABCIQueryWithOptions(fmt.Sprintf("custom/%s/%s/%s", otypes.StoreKey, otypes.QueryData, hash), nil, rpcclient.ABCIQueryOptions{})
		if err != nil {
			l.Error(":exploding_head: Failed to get data source with error: %s", err.Error())
			return nil, err
		}
		resValue = res.Response.GetValue()
		c.fileCache.AddFile(resValue)
	} else {
		l.Debug(":card_file_box: Found data source hash: %s in cache file", hash)
	}

	l.Debug(":balloon: Received data source hash: %s content: %q", hash, resValue[:32])
	return resValue, nil
}
