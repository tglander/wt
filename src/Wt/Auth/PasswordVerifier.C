/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <string>

#include "Wt/WLogger"
#include "AuthUtils.h"
#include "HashFunction"
#include "PasswordHash"
#include "PasswordVerifier"

namespace Wt {

LOGGER("Auth::PasswordVerifier");

  namespace Auth {

PasswordVerifier::PasswordVerifier()
  : saltLength_(12)
{ }

PasswordVerifier::~PasswordVerifier()
{ 
  for (unsigned i = 0; i < hashFunctions_.size(); ++i)
    delete hashFunctions_[i];
}

void PasswordVerifier::addHashFunction(HashFunction *function)
{
  hashFunctions_.push_back(function);
}

bool PasswordVerifier::needsUpdate(const PasswordHash& hash) const
{
  return hash.function() != hashFunctions()[0]->name();
}

PasswordHash PasswordVerifier::hashPassword(const WString& password) const
{
  std::string msg = password.toUTF8();
  std::string salt = Utils::createSalt(saltLength_);
  salt = Utils::encodeAscii(salt);

  const HashFunction& f = *hashFunctions_[0];
  std::string hash = f.compute(msg, salt);
  return PasswordHash(f.name(), salt, hash);
}

bool PasswordVerifier::verify(const WString& password,
			      const PasswordHash& hash) const
{
  for (unsigned i = 0; i < hashFunctions_.size(); ++i) {
    const HashFunction& f = *hashFunctions_[i];

    if (f.name() == hash.function())
      return f.verify(password.toUTF8(), hash.salt(), hash.value());
  }

  LOG_ERROR("verify() no hash configured for " << hash.function());

  return false;
}

  }
}
