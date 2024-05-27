from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ConanPackage(ConanFile):
    name = "network-monitor"
    version = "0.1.0"
    
    generators = ("CMakeToolchain", "CMakeDeps")
    settings = ("os", "compiler", "build_type", "arch")
    
    def requirements(self):
        self.requires("boost/1.85.0")
        self.requires("openssl/3.2.1")
    
    def configure(self):
        self.options["boost/*"].shared = False

    def layout(self):
        cmake_layout(self)
