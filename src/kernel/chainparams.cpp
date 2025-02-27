// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kernel/chainparams.h>

#include <chainparamsseeds.h>
#include <consensus/amount.h>
#include <consensus/merkle.h>
#include <consensus/params.h>
#include <hash.h>
#include <kernel/messagestartchars.h>
#include <logging.h>
#include <primitives/block.h>
#include <primitives/transaction.h>
#include <script/interpreter.h>
#include <script/script.h>
#include <uint256.h>
#include <util/chaintype.h>
#include <util/strencodings.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <type_traits>

using namespace util::hex_literals;

// Workaround MSVC bug triggering C7595 when calling consteval constructors in
// initializer lists.
// A fix may be on the way:
// https://developercommunity.visualstudio.com/t/consteval-conversion-function-fails/1579014
#if defined(_MSC_VER)
auto consteval_ctor(auto&& input) { return input; }
#else
#define consteval_ctor(input) (input)
#endif

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.version = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(MakeTransactionRef(std::move(txNew)));
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = BlockMerkleRoot(genesis);
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database.
 *
 * CBlock(hash=000000000019d6, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=4a5e1e, nTime=1231006505, nBits=1d00ffff, nNonce=2083236893, vtx=1)
 *   CTransaction(hash=4a5e1e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
 *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d0104455468652054696d65732030332f4a616e2f32303039204368616e63656c6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f757420666f722062616e6b73)
 *     CTxOut(nValue=50.00000000, scriptPubKey=0x5F1DF16B2B704C8A578D0B)
 *   vMerkleTree: 4a5e1e
 */
static CBlock CreateGenesisBlock(uint32_t nTime, uint32_t nNonce, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "BitScript - Blockchain Storage - UK 01/01/2025";
    const CScript genesisOutputScript = CScript() << "0470ca1d8977c6943640139e6d5b68d56a7850a98467b92d310fc2fb93abf0866b9250ac509c78712c62f8d987d4b8d9ad6951bd746ad384ab10d4fec852d03ddd"_hex << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nBits, nVersion, genesisReward);
}

/**
 * Main network on which people trade goods and services.
 */
class CMainParams : public CChainParams {
    public:
        CMainParams() {
            m_chain_type = ChainType::MAIN;
            consensus.signet_blocks = false;
            consensus.signet_challenge.clear();
            consensus.nSubsidyHalvingInterval = 5000;
            consensus.script_flag_exceptions.emplace(// BIP16 exception
                uint256{"00000177633cc307cd7f8630cf89dea7bea821a426ef28022423cbbe518aa546"}, SCRIPT_VERIFY_NONE);
            consensus.script_flag_exceptions.emplace( // Taproot exception
                uint256{"000000028c03020ddf3d4772c1bdb98f161b06a0eb879fa1d4665bcc62dcb0fe"}, SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS);
            consensus.BIP34Height = 227931;
            consensus.BIP34Hash = uint256{"0000086bf866c6c7054b748b9fc269c28c2d4654f720e7768b1e140b287debde"};
            consensus.BIP65Height = 388; // 
            consensus.BIP66Height = 363; // 
            consensus.CSVHeight = 419; // 
            consensus.SegwitHeight = 481; // 
            consensus.MinBIP9WarningHeight = 483; // segwit activation height + miner confirmation window
            consensus.powLimit = uint256("00000fffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
            consensus.nPowTargetTimespan = 144 * 60; // 144 blocks = 144 minutes (2Hours) retarget 
            consensus.nPowTargetSpacing = 60; // 1 minute
            consensus.fPowAllowMinDifficultyBlocks = false;
            consensus.fPowNoRetargeting = false;
            consensus.nRuleChangeActivationThreshold = 90; // 90% of 100
            consensus.nMinerConfirmationWindow = 100; // nPowTargetTimespan / nPowTargetSpacing
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 1; 
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 1737429115; // Time block (height 723)
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = 1737429115 + 3600; // ~50 blocks
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 50; // 77 blocks on future
    
            // Deployment of Taproot (BIPs 340-342)
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1737409058;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 1737409058 + 18000; // 5 hours after started
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 1000; // Activate at block 1000
    
            consensus.nMinimumChainWork = uint256("00000000000000000000000000000000000000000000000000001b1f9ed6f1c6");
            consensus.defaultAssumeValid = uint256("00000001eba2d16d93dd5638d8d914bea08f44340cf1ead81c3b3671392e3188"); // 50888
    
            pchMessageStart[0] = 0xf8;
            pchMessageStart[1] = 0xbc;
            pchMessageStart[2] = 0xb3;
            pchMessageStart[3] = 0xd7;
            nDefaultPort = 7333;
            nPruneAfterHeight = 100000;
            m_assumed_blockchain_size = 0;
            m_assumed_chain_state_size = 0;
    
            genesis = CreateGenesisBlock(1735689606, 73204, 0x1e0ffff0, 1, 5000000 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256("000000722d67deca36a08defdba9de43ffd698198bf1a9876b690475e14eb0b5"));
            assert(genesis.hashMerkleRoot == uint256("6b871c79b17729663ef8c6da282cdf411481cf22dd92f0721e88c4ce76f2d6ae"));
            
            vFixedSeeds.clear();
    
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 25);  // Prefixo para endereços públicos (letra B)
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 15);  // Prefixo para endereços de scripts (numero 7)
            base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 128);      // Prefixo para chaves secretas (numero 80)
    
            base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x88, 0xB2, 0x1E};
            base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x88, 0xAD, 0xE4};
            bech32_hrp = "bs";
    
            vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_main), std::end(chainparams_seed_main));
    
            fDefaultConsistencyChecks = false;
            m_is_mockable_chain = false;
    
            checkpointData = {
                {
                    {0, uint256("000000722d67deca36a08defdba9de43ffd698198bf1a9876b690475e14eb0b5")},
                    {10000, uint256("00000005daf15f9f3be63b7d2da8b06878c6f242544b0290b823225f179be6d0")},
                    {37820, uint256("0000000388ea37d1328b6ceaa268a414bdc6ae532cd3435ba0448b21a02f6bde")},
                    {50892, uint256("00000001eba2d16d93dd5638d8d914bea08f44340cf1ead81c3b3671392e3188")}
                }
            };
    
            m_assumeutxo_data = {
                {
                    .height = 37'817,
                    .hash_serialized = AssumeutxoHash{uint256{"f5bfbfe09dc9d04e248cd9c72819533d022f250c9a4add3afa7428f39190205d"}}, 
                    .m_chain_tx_count = 114629, // Total de transações até esse bloco
                    .blockhash = consteval_ctor(uint256{"00000004d5055e3f69a083c997b78f8a1b148979cfca7d07c30813aa243ec443"}),
                }
            };
    
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000000000000000000011c5890365bdbe5d25b97ce0057589acaef4f1a57263f
                .nTime    = 1740654297,
                .tx_count = 127836,
                .dTxRate  = 0.01541361685625738,
            };
        }
    };
    
/**
 * Testnet (v3): public test network which is reset from time to time.
 */
class CTestNetParams : public CChainParams {
    public:
        CTestNetParams() {
            m_chain_type = ChainType::TESTNET;
            consensus.signet_blocks = false;
            consensus.signet_challenge.clear();
            consensus.nSubsidyHalvingInterval = 5000;
            consensus.script_flag_exceptions.emplace( // BIP16 exception
            uint256(""), SCRIPT_VERIFY_NONE);
            consensus.BIP34Height = 227;
            consensus.BIP34Hash = uint256("");
            consensus.BIP65Height = 388; // 
            consensus.BIP66Height = 363; // 
            consensus.CSVHeight = 419; // 
            consensus.SegwitHeight = 481; // 
            consensus.MinBIP9WarningHeight = 483; // segwit activation height + miner confirmation window
            consensus.powLimit = uint256("00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
            consensus.nPowTargetTimespan = 144 * 60; // 144 blocos = 144 minutos (2 horas) de intervalo para ajuste de dificuldade
            consensus.nPowTargetSpacing = 60; // 1 minuto entre blocos
            consensus.fPowAllowMinDifficultyBlocks = true;
            consensus.fPowNoRetargeting = false;
            consensus.nRuleChangeActivationThreshold = 90; // 75% for testchains
            consensus.nMinerConfirmationWindow = 100; // nPowTargetTimespan / nPowTargetSpacing
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
    
            // Deployment of Taproot (BIPs 340-342)
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = 1737409058; // Timestamp do bloco atual
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = 1737409058 + 18000; // 5 horas após o início
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 1000; // Ativar no bloco 1000
    
            consensus.nMinimumChainWork = uint256S("0x");
            consensus.defaultAssumeValid = uint256S("0x"); // 2344474
    
            pchMessageStart[0] = 0x0b;
            pchMessageStart[1] = 0x11;
            pchMessageStart[2] = 0x09;
            pchMessageStart[3] = 0x07;
            nDefaultPort = 17333;
            nPruneAfterHeight = 1000;
            m_assumed_blockchain_size = 93;
            m_assumed_chain_state_size = 19;
    
            genesis = CreateGenesisBlock(1735689606, 73204, 0x1e0ffff0, 1, 5000000 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256S("0x000000722d67deca36a08defdba9de43ffd698198bf1a9876b690475e14eb0b5"));
            assert(genesis.hashMerkleRoot == uint256S("0x6b871c79b17729663ef8c6da282cdf411481cf22dd92f0721e88c4ce76f2d6ae"));
    
            vFixedSeeds.clear();
            vSeeds.clear();
    
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
            base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
            base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
    
            bech32_hrp = "btb";
    
            vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_test), std::end(chainparams_seed_test));
    
            fDefaultConsistencyChecks = false;
            m_is_mockable_chain = false;
    
            checkpointData = {
                {
                    {546, uint256{"000000002a936ca763904c3c35fce2f3556c559c0214345d31b1bcebf76acb70"}},
                }
            };
    
            m_assumeutxo_data = {
                // TODO to be specified in a future patch.
               };
    
            chainTxData = ChainTxData{
                // Data from RPC: getchaintxstats 4096 000000000000000465b1a66c9f386308e8c75acef9201f3f577811da09fc90ad
                .nTime    = 1723613341,
                .tx_count = 187917082,
                .dTxRate  = 3.265051477698455,
            };
        }
    };
    
/**
 * Testnet (v4): public test network which is reset from time to time.
 */
class CTestNet4Params : public CChainParams {
public:
    CTestNet4Params() {
        m_chain_type = ChainType::TESTNET4;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 5000;
        consensus.BIP34Height = 1;
        consensus.BIP34Hash = uint256{};
        consensus.BIP65Height = 1;
        consensus.BIP66Height = 1;
        consensus.CSVHeight = 1;
        consensus.SegwitHeight = 1;
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"00000000ffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 144 * 60; // 144 blocos = 144 minutos (2 horas) de intervalo para ajuste de dificuldade
        consensus.nPowTargetSpacing = 60; // 1 minuto entre blocos
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = true;
        consensus.fPowNoRetargeting = false;
        consensus.nRuleChangeActivationThreshold = 90; 
        consensus.nMinerConfirmationWindow = 100; 
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        // Deployment of Taproot (BIPs 340-342)
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{}; 

        pchMessageStart[0] = 0x1c;
        pchMessageStart[1] = 0x16;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x28;
        nDefaultPort = 47333;
        nPruneAfterHeight = 1000;
        m_assumed_blockchain_size = 1;
        m_assumed_chain_state_size = 0;

        const char* testnet4_genesis_msg = "BitScript - Blockchain Storage - UK 01/01/2025";
        const CScript testnet4_genesis_script = CScript() << "000000000000000000000000000000000000000000000000000000000000000000"_hex << OP_CHECKSIG;
        genesis = CreateGenesisBlock(testnet4_genesis_msg, testnet4_genesis_script, 1735689727, 167876, 0x1e0ffff0, 1, 5000000 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256{"6b871c79b17729663ef8c6da282cdf411481cf22dd92f0721e88c4ce76f2d6ae"});
        assert(genesis.hashMerkleRoot == uint256{"7aa0a7ae1e223414cb807e40cd57e667b718e42aaf9306db9102fe28912b7b4e"});

        vFixedSeeds.clear();
        vSeeds.clear();
        // nodes with support for servicebits filtering should be at the top

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "btb";

        vFixedSeeds = std::vector<uint8_t>(std::begin(chainparams_seed_testnet4), std::end(chainparams_seed_testnet4));

        fDefaultConsistencyChecks = false;
        m_is_mockable_chain = false;

        checkpointData = {
            {
                {},
            }
        };

        m_assumeutxo_data = {
            {}
        };

        chainTxData = ChainTxData{
            // Data from RPC: getchaintxstats 4096 000000005be348057db991fa5d89fe7c4695b667cfb311391a8db374b6f681fd
            .nTime    = 1735689727,
            .tx_count = 0,
            .dTxRate  = 0,
        };
    }
};

/**
 * Signet: test network with an additional consensus parameter (see BIP325).
 */
class SigNetParams : public CChainParams {
    public:
        explicit SigNetParams(const SigNetOptions& options)
        {
            std::vector<uint8_t> bin;
            vSeeds.clear();
    
            if (!options.challenge) {
                bin = ParseHex("5121903a57b3aae64bf6aad80e0cd44a336cb8d4235a30f3421b731b8719f2227b3c772ab16810e2269eb097a452d349bc0b094577d059cf160c5b36e0ae40220856211baa828a20d00d2e2d9ffc637a720e7462bc82c9f272d361972cb39542f9c38b4bbfceb7919c80cef91c8940ac27f7fc6430c7c49a4b58dadec061c5ed5c221e52ae");
                consensus.nMinimumChainWork = uint256("");
                consensus.defaultAssumeValid = uint256(""); // Altere com o hash do bloco válido inicial
                m_assumed_blockchain_size = 5;
                m_assumed_chain_state_size = 1;
                chainTxData = ChainTxData{
                    .nTime    = 1735689688, // Atualize com o timestamp inicial
                    .nTxCount = 0,
                    .dTxRate  = 0.0,
                };
            } else {
                bin = *options.challenge;
                consensus.nMinimumChainWork = uint256{};
                consensus.defaultAssumeValid = uint256{};
                m_assumed_blockchain_size = 0;
                m_assumed_chain_state_size = 0;
                chainTxData = ChainTxData{
                    0,
                    0,
                    0,
                };
                LogPrintf("Signet with challenge %s\n", HexStr(bin));
            }
    
            if (options.seeds) {
                vSeeds = *options.seeds;
            }
    
            m_chain_type = ChainType::SIGNET;
            consensus.signet_blocks = true;
            consensus.signet_challenge.assign(bin.begin(), bin.end());
            consensus.nSubsidyHalvingInterval = 210000;
            consensus.BIP34Height = 1;
            consensus.BIP34Hash = uint256{};
            consensus.BIP65Height = 1;
            consensus.BIP66Height = 1;
            consensus.CSVHeight = 1;
            consensus.SegwitHeight = 1;
            consensus.nPowTargetTimespan = 2 * 60 * 60; // two weeks
            consensus.nPowTargetSpacing = 60;
            consensus.fPowAllowMinDifficultyBlocks = false;
            consensus.enforce_BIP94 = false;
            consensus.fPowNoRetargeting = false;
            consensus.nRuleChangeActivationThreshold = 108; // 90% of 2016
            consensus.nMinerConfirmationWindow = 144; // nPowTargetTimespan / nPowTargetSpacing
            consensus.MinBIP9WarningHeight = 0;
            consensus.powLimit = uint256{"00000377ae000000000000000000000000000000000000000000000000000000"};
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 28;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = Consensus::BIP9Deployment::NEVER_ACTIVE;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
            consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay
    
            // Activation of Taproot (BIPs 340-342)
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
            consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay
    
            // message start is defined as the first 4 bytes of the sha256d of the block script
            HashWriter h{};
            h << consensus.signet_challenge;
            uint256 hash = h.GetHash();
            std::copy_n(hash.begin(), 4, pchMessageStart.begin());
    
            nDefaultPort = 38333;
            nPruneAfterHeight = 1000;
    
            genesis = CreateGenesisBlock(1735689688, 290527, 0x1e0377ae, 1, 5000000 * COIN);
            consensus.hashGenesisBlock = genesis.GetHash();
            assert(consensus.hashGenesisBlock == uint256("0000024b2e9e97e9a6e41ff2457993352318d12018f36cdc5585eeb4bb670532"));
            assert(genesis.hashMerkleRoot == uint256("6b871c79b17729663ef8c6da282cdf411481cf22dd92f0721e88c4ce76f2d6ae"));
    
            vFixedSeeds.clear();
    
            m_assumeutxo_data = {
    
            };
    
            base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
            base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
            base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
            base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
            base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};
    
            bech32_hrp = "tbs";
    
            fDefaultConsistencyChecks = false;
            m_is_mockable_chain = false;
        }
    };
    
/**
 * Regression test: intended for private networks only. Has minimal difficulty to ensure that
 * blocks can be found instantly.
 */
class CRegTestParams : public CChainParams
{
public:
    explicit CRegTestParams(const RegTestOptions& opts)
    {
        m_chain_type = ChainType::REGTEST;
        consensus.signet_blocks = false;
        consensus.signet_challenge.clear();
        consensus.nSubsidyHalvingInterval = 150;
        consensus.BIP34Height = 1; // Always active unless overridden
        consensus.BIP34Hash = uint256();
        consensus.BIP65Height = 1;  // Always active unless overridden
        consensus.BIP66Height = 1;  // Always active unless overridden
        consensus.CSVHeight = 1;    // Always active unless overridden
        consensus.SegwitHeight = 0; // Always active unless overridden
        consensus.MinBIP9WarningHeight = 0;
        consensus.powLimit = uint256{"7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"};
        consensus.nPowTargetTimespan = 144 * 60; // 144 blocos = 144 minutos (2 horas) de intervalo para ajuste de dificuldade
        consensus.nPowTargetSpacing = 60; // 1 minuto entre blocos
        consensus.fPowAllowMinDifficultyBlocks = true;
        consensus.enforce_BIP94 = opts.enforce_bip94;
        consensus.fPowNoRetargeting = true;
        consensus.nRuleChangeActivationThreshold = 108; // 75% for testchains
        consensus.nMinerConfirmationWindow = 144; // Faster than normal for regtest (144 instead of 2016)

        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].bit = 1;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nStartTime = 0;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TESTDUMMY].min_activation_height = 0; // No activation delay

        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].bit = 2;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nStartTime = Consensus::BIP9Deployment::ALWAYS_ACTIVE;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].nTimeout = Consensus::BIP9Deployment::NO_TIMEOUT;
        consensus.vDeployments[Consensus::DEPLOYMENT_TAPROOT].min_activation_height = 0; // No activation delay

        consensus.nMinimumChainWork = uint256{};
        consensus.defaultAssumeValid = uint256{};

        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0xbf;
        pchMessageStart[2] = 0xb5;
        pchMessageStart[3] = 0xda;
        nDefaultPort = 17444;
        nPruneAfterHeight = opts.fastprune ? 100 : 1000;
        m_assumed_blockchain_size = 0;
        m_assumed_chain_state_size = 0;

        for (const auto& [dep, height] : opts.activation_heights) {
            switch (dep) {
            case Consensus::BuriedDeployment::DEPLOYMENT_SEGWIT:
                consensus.SegwitHeight = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_HEIGHTINCB:
                consensus.BIP34Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_DERSIG:
                consensus.BIP66Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CLTV:
                consensus.BIP65Height = int{height};
                break;
            case Consensus::BuriedDeployment::DEPLOYMENT_CSV:
                consensus.CSVHeight = int{height};
                break;
            }
        }

        for (const auto& [deployment_pos, version_bits_params] : opts.version_bits_parameters) {
            consensus.vDeployments[deployment_pos].nStartTime = version_bits_params.start_time;
            consensus.vDeployments[deployment_pos].nTimeout = version_bits_params.timeout;
            consensus.vDeployments[deployment_pos].min_activation_height = version_bits_params.min_activation_height;
        }

        genesis = CreateGenesisBlock(1735689702, 3, 0x207fffff, 1, 5000000 * COIN);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256("361a9b4dbb07235de1548fab66057824f15415c407361f2a97a5e23f708fa154"));
        assert(genesis.hashMerkleRoot == uint256("06b871c79b17729663ef8c6da282cdf411481cf22dd92f0721e88c4ce76f2d6ae"));

        vFixedSeeds.clear(); //!< Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();

        fDefaultConsistencyChecks = true;
        m_is_mockable_chain = true;

        checkpointData = {
            {
                {0, uint256{"361a9b4dbb07235de1548fab66057824f15415c407361f2a97a5e23f708fa154"}},
            }
        };

        m_assumeutxo_data = {
            
        };

        chainTxData = ChainTxData{
            0,
            0,
            0
        };

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,111);
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1,196);
        base58Prefixes[SECRET_KEY] =     std::vector<unsigned char>(1,239);
        base58Prefixes[EXT_PUBLIC_KEY] = {0x04, 0x35, 0x87, 0xCF};
        base58Prefixes[EXT_SECRET_KEY] = {0x04, 0x35, 0x83, 0x94};

        bech32_hrp = "bsrt";
    }
};

std::unique_ptr<const CChainParams> CChainParams::SigNet(const SigNetOptions& options)
{
    return std::make_unique<const SigNetParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::RegTest(const RegTestOptions& options)
{
    return std::make_unique<const CRegTestParams>(options);
}

std::unique_ptr<const CChainParams> CChainParams::Main()
{
    return std::make_unique<const CMainParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet()
{
    return std::make_unique<const CTestNetParams>();
}

std::unique_ptr<const CChainParams> CChainParams::TestNet4()
{
    return std::make_unique<const CTestNet4Params>();
}

std::vector<int> CChainParams::GetAvailableSnapshotHeights() const
{
    std::vector<int> heights;
    heights.reserve(m_assumeutxo_data.size());

    for (const auto& data : m_assumeutxo_data) {
        heights.emplace_back(data.height);
    }
    return heights;
}

std::optional<ChainType> GetNetworkForMagic(const MessageStartChars& message)
{
    const auto mainnet_msg = CChainParams::Main()->MessageStart();
    const auto testnet_msg = CChainParams::TestNet()->MessageStart();
    const auto testnet4_msg = CChainParams::TestNet4()->MessageStart();
    const auto regtest_msg = CChainParams::RegTest({})->MessageStart();
    const auto signet_msg = CChainParams::SigNet({})->MessageStart();

    if (std::ranges::equal(message, mainnet_msg)) {
        return ChainType::MAIN;
    } else if (std::ranges::equal(message, testnet_msg)) {
        return ChainType::TESTNET;
    } else if (std::ranges::equal(message, testnet4_msg)) {
        return ChainType::TESTNET4;
    } else if (std::ranges::equal(message, regtest_msg)) {
        return ChainType::REGTEST;
    } else if (std::ranges::equal(message, signet_msg)) {
        return ChainType::SIGNET;
    }
    return std::nullopt;
}
