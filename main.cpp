/*
 * X Window Manager 
 * 
 * (c) Naufal Adriansyah 2025. Use at your own risk :D
 */

#include <cstdlib>
#include <wm.hpp>

int main(int argc, const char* argv[]) {
  std::unique_ptr<WindowManager> wm(WindowManager::create());
  if (!wm) {
    std::cerr << "Failed to initiate window manager\n";
    return EXIT_FAILURE;
  }
  
  wm->run();

  return EXIT_SUCCESS;
}
