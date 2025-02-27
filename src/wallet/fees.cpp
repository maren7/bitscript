// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2022 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <wallet/fees.h>

#include <wallet/coincontrol.h>
#include <wallet/wallet.h>


namespace wallet {

    // Função que retorna a taxa exigida para a transação
    CAmount GetRequiredFee(const CWallet& wallet, unsigned int nTxBytes)
    {
        return 0; // Zerar a taxa necessária
    }
    
    // Função que retorna a taxa mínima para a transação
    CAmount GetMinimumFee(const CWallet& wallet, unsigned int nTxBytes, const CCoinControl& coin_control, FeeCalculation* feeCalc)
    {
        return 0; // Zerar a taxa mínima
    }
    
    // Função que retorna a taxa exigida (rate) para a transação
    CFeeRate GetRequiredFeeRate(const CWallet& wallet)
    {
        return CFeeRate(0); // Garantir que a taxa exigida seja zero
    }
    
    // Função que retorna a taxa mínima (rate) para a transação
    CFeeRate GetMinimumFeeRate(const CWallet& wallet, const CCoinControl& coin_control, FeeCalculation* feeCalc)
    {
        return CFeeRate(0); // Garantir que a taxa mínima seja zero
    }
    
    // Função que retorna a taxa de descarte
    CFeeRate GetDiscardRate(const CWallet& wallet)
    {
        return CFeeRate(0); // Garantir que a taxa de descarte seja zero
    }
    
    } // namespace wallet    