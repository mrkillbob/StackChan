#pragma once
// Dependency-free backend contract; no UI, phone credentials or vendor APIs.
namespace network_setup {
bool CredentialPersistenceAllowed();
void NotifyCredentialsSaved(bool success);
bool CredentialSaveSucceeded();
bool NetworkWasStarted();
}
