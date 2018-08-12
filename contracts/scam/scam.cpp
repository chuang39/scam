#include "scam.hpp"

void scam::createpool(const name owner, const string poolname) {
    require_auth(_self);

    // TODO: should only allow one active pool
    // TODO: add a method to control pool state
    print( "Create pool ", poolname, " by owner= ", name{owner} );

    pools.emplace(owner, [&](auto &pool) {
        pool.id = pools.available_primary_key();
        pool.poolname = string(poolname);
        pool.owner = name{owner};
        pool.lastbuyer = name{owner};
        pool.status = 1;
        pool.created_at = now();
        pool.end_at = now() + 24 * 3600;
        pool.key_balance = 10;
        pool.eos_balance = asset(0, symbol_type(S(4, EOS)));
        pool.key_price = asset(1220, symbol_type(S(4, EOS)));
    });
    for( const auto& pool : pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
}

void scam::deposit(const currency::transfer &t, account_name code) {

/*
    if(from == _self) {
        return;
    }
    require_auth(_self);
    print("\n>>> sender >>>", from, " - name: ", name{from});

    eosio_assert(quantity.symbol == string_to_symbol(4, "EOS"), "Only accepts EOS for deposits");
    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "Quantity must be positive");

    // find account
    auto itr_acnt = accounts.find(from);
    if(itr_acnt != accounts.end()) {
        accounts.modify(itr_acnt, _self, [&](auto& r){
            // Assumption: total currency issued by eosio.token will not overflow asset
            r.balance += transfer_data.quantity;
            new_balance = r.balance;
        });
    } else {
        accounts.emplace(_self, [&](auto& r){
            r.balance = transfer_data.quantity;
            new_balance = r.balance;
        });
    }

*/


}

//@abi action
void scam::withdraw(const account_name to) {
    require_auth(to);


/*

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(code == N(eosio.token);
    eosio_assert(quantity.amount > 0, "must withdraw positive quantity");

    auto itr = accounts.find(to);
    eosio_assert(itr != accounts.end(), "unknown account");

    accounts.modify( itr, 0, [&]( auto& acnt ) {
        eosio_assert( acnt.eos_balance >= quantity, "insufficient balance" );
        acnt.eos_balance -= quantity;
    });


    if( itr->is_empty() ) {
        accounts.erase(itr);
    }
    */
}

void scam::runwithdraw(const account_name to) {
    print(">>> withdraw:", to);

}

void scam::deleteall() {
    require_auth(_self);

    auto itr = pools.begin();
    while (itr != pools.end()) {
        itr = pools.erase(itr);
    }

    auto itr_acnt = accounts.begin();
    while (itr_acnt != accounts.end()) {
        itr_acnt = accounts.erase(itr_acnt);
    }
}

void scam::reset() {
    require_auth(_self);

}

void scam::apply(account_name contract, account_name act) {
    print(">>> apply:", contract, ">>> act:", act);
    if (act == N(transfer)) {
        deposit(unpack_action_data<currency::transfer>(), contract);
        return;
    }

    if (act == N(withdraw)) {
        runwithdraw(contract);
        return;
    }

    if (contract != _self) {
        return;
    }

    auto &thiscontract = *this;
    OSIO_ABI(scam, (deposit)(createpool)(deleteall)(reset));
}

EOSIO_ABI_EX(scam, (deposit)(withdraw)(createpool)(deleteall)(reset))
