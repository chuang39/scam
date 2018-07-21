//
// Created by Kevin Huang on 7/20/18.
//

#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include "scam.hpp"


using namespace eosio;
using std::string;
using std::hash;

void scam::createTransaction(name owner, string pet_name) {

    require_auth(owner);

    //uuid new_id = _next_id();

    transactions.emplace(owner, [&](auto &r) {
        st_transactions transaction{};
        transaction.id = 1;
        transaction.name = "baby";
        transaction.owner = owner;
        transaction.created_at = now();
        transaction.ammount = 0;

        //pet.type = (hash_str(pet_name) + pet.created_at + pet.id + owner) % pc.last_pet_type_id;

        r = transaction;
    });


    auto owner_transactions = transactions.get_index<N(byowner)>();
}

#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( action == N(onerror)) { \
         /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ \
         eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
      } \
      auto self = receiver; \
      if( code == self || code == N(eosio.token) || action == N(onerror) ) { \
         TYPE thiscontract( self ); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
         /* does not allow destructor of thiscontract to run: eosio_exit(0); */ \
      } \
   } \
}

EOSIO_ABI_EX(scam, (createTransaction))