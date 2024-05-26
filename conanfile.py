from conan import ConanFile

class ConanPackage(ConanFile):
    name = "network-monitor"
    version = "0.1.0"
    
    generators = ["CMakeToolchain", "CMakeDeps"]
    settings = "build_type"
    
    requires = []
