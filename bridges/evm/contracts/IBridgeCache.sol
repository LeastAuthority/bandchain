pragma solidity 0.5.14;
pragma experimental ABIEncoderV2;

import {IBridge} from "./Bridge.sol";

interface IBridgeCache {
    /// Returns the ResponsePacket for a given RequestPacket.
    /// @param _request The tuple that represents RequestPacket struct.
    function getLatestResponse(IBridge.RequestPacket calldata _request)
        external
        view
        returns (IBridge.ResponsePacket memory);
}
