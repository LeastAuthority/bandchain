pragma solidity 0.5.14;

/// @dev Library for performing signer recovery for ECDSA secp256k1 signature. Note that the
/// library is written specifically for signature signed on Tendermint's precommit data, which
/// includes the block hash and some additional information prepended and appended to the block
/// hash. The prepended part (prefix) is the same for all the signers, while the appended part
/// (suffix) is different for each signer (including machine clock, validator index, etc).
library TMSignature {
    struct Data {
        bytes32 r;
        bytes32 s;
        uint8 v;
        bytes signedDataPrefix;
        bytes signedDataSuffix;
    }

    /// @dev Returns the address that signed on the given block hash.
    /// @param _blockHash The block hash that the validator signed data on.
    function recoverSigner(Data memory _self, bytes32 _blockHash)
        internal
        pure
        returns (address)
    {
        return
            ecrecover(
                sha256(
                    abi.encodePacked(
                        _self.signedDataPrefix,
                        _blockHash,
                        _self.signedDataSuffix
                    )
                ),
                _self.v,
                _self.r,
                _self.s
            );
    }
}
