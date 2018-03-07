#ifndef TYPES_HPP
#define TYPES_HPP

#include <vector>
using std::vector;
#include <map>
using std::map;

namespace blockchain {
  class Pubkey {
    int x = 0; // just for now
  public:
    Pubkey() {} // just for now
    Pubkey(int x): x(x) {} // just for now
    bool operator==(Pubkey) const;
    bool operator<(Pubkey) const;
  };
  class Hash {};
  class ExtraChainData;
  class Validable;
  typedef map<const Validable*,bool> ValidsChecked;
  class Validable {
  public:
    virtual bool getValid(const ExtraChainData&,ValidsChecked&) const = 0;
  };
  class Applyable {
  public:
    virtual void apply(ExtraChainData&) const = 0;
    virtual void unapply(ExtraChainData&) const = 0;
  };
  class Hashable {
  public:
    virtual Hash getHash() const = 0;
    virtual Hash getHashBeforeSig() const { return Hash(); } // only write this func if thing is signed
  };
  class Sig: public Hashable {
    Pubkey pubkey;
    // more data here
  public:
    Sig(Pubkey);
    Pubkey getPerson() const;
    virtual bool getValid(const Hashable&) const;
    virtual Hash getHash() const;
  };
  class Txn;
  typedef unsigned int TxnAmt;
  typedef unsigned int GasAmt;
  class TxnOtp: public Hashable, public Validable {
    Pubkey person;
    TxnAmt amt;
  public:
    TxnOtp(Pubkey,TxnAmt,ValidsChecked* = NULL /*only give a value if it's origin*/);
    TxnAmt getAmt() const;
    Pubkey getPerson() const;
    virtual Hash getHash() const;
    virtual bool getValid(const ExtraChainData&,ValidsChecked&) const;
  };
  class StorageChange: public Applyable {
    Pubkey person;
    unsigned int location;
    unsigned int value;
    unsigned int prevValue;
  public:
    StorageChange(Pubkey,unsigned int,unsigned int,unsigned int);
    StorageChange(Pubkey,unsigned int,unsigned int,const ExtraChainData&);
    Pubkey getPerson() const;
    unsigned int getLocation() const;
    unsigned int getValue() const;
    unsigned int getPrevValue() const;
    virtual void apply(ExtraChainData&) const;
    virtual void unapply(ExtraChainData&) const;
  };
  class Txn: public Hashable, public Validable, public Applyable {
    vector<const TxnOtp*> inps;
    vector<TxnOtp> otps;
    vector<Sig> sigs;
  public:
    Txn(vector<const TxnOtp*>,vector<TxnOtp>,vector<Sig>);
    virtual Hash getHash() const;
    virtual Hash getHashBeforeSig() const;
    virtual bool getValid(const ExtraChainData&,ValidsChecked&) const;
    virtual void apply(ExtraChainData&) const;
    virtual void unapply(ExtraChainData&) const;
    const vector<TxnOtp>& getOtps() const;
  };
  class Block: public Hashable, public Validable, public Applyable {
    vector<Txn> txns;
    vector<Block*> approved;
    int nonce;
  public:
    Block(vector<Txn>,vector<Block*>);
    virtual Hash getHash() const;
    virtual bool getValid(const ExtraChainData&,ValidsChecked&) const;
    virtual void apply(ExtraChainData&) const;
    virtual void unapply(ExtraChainData&) const;
    const vector<Txn>& getTxns() const;
  };
  class ExtraChainData {
  public:
    map<const TxnOtp*,const Txn*> spentOutputs;
    map<Pubkey,map<unsigned int,unsigned int>> storage;
  };
}

#endif
