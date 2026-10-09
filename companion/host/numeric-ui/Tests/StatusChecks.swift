import Foundation
@main struct Checks {
 static func main() {
  func valid(_ s: String) -> Bool { PairingStatus.decode(Data(s.utf8)) != nil }
  assert(valid("{\"state\":\"comparing\",\"code\":\"012345\"}"))
  assert(valid("{\"state\":\"paired\",\"acknowledged\":true}"))
  assert(!valid("{\"state\":\"paired\"}"))
  assert(!valid("{\"state\":\"comparing\",\"code\":\"12345\"}"))
  assert(!valid("{\"state\":\"comparing\",\"code\":\"１２３４５６\"}"))
  assert(!valid("{\"state\":\"paired\",\"code\":\"123456\",\"acknowledged\":true}"))
  assert(!valid("{\"state\":\"comparing\",\"credential\":\"synthetic-no-authority\"}"))
  assert(!valid("{\"state\":\"idle\",\"remaining\":true}"))
  assert(!valid("{\"state\":\"idle\",\"remaining\":120.5}"))
  assert(!valid("{\"state\":\"idle\",\"remaining\":121}"))
  assert(valid("{\"state\":\"idle\",\"remaining\":120}"))
  assert(!valid("{\"state\":\"paired\",\"acknowledged\":1}"))
  assert(!valid("{\"state\":\"unknown\"}"))
  assert(!valid(String(repeating:" ",count:1025)))
  print("PASS14 nativeUI status/secret-field/type/expiry checks; nohardwareIO")
 }
}
