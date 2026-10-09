#pragma once
namespace stock_shell {
void SetSetupContext(bool active);
void SaveSetupReturnForReset();
bool ConsumeSetupReturn();
}
