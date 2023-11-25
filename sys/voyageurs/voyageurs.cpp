
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>

#include "keyboard/keyboard_driver.h"
#include "voyageurs_server.h"

uint64_t main(uint64_t init_port) {
  ParseInitPort(init_port);

  dbgln("Initializing PS/2 Driver.");
  KeyboardDriver driver;

  dbgln("Starting PS/2 Thread.");
  Thread keyboard_thread = driver.StartInterruptLoop();

  dbgln("Starting voyaguers server.");
  ASSIGN_OR_RETURN(glcr::UniquePtr<VoyageursServer> server,
                   VoyageursServer::Create(driver));

  Thread server_thread = server->RunServer();

  check(server_thread.Join());
  check(keyboard_thread.Join());

  return glcr::OK;
}
