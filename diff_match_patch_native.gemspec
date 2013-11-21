Gem::Specification.new do |s|
  s.name    = "diff_match_patch_native"
  s.version = "2.0.0-rc1"
  s.author  = "Elliot Laster"
  s.email       = "elliotlaster@gmail.com"
  s.homepage    = "https://github.com/elliotlaster/Ruby-Diff-Match-Patch"
  s.summary     = "Ruby bindings for the C++ version of google-diff-match-patch: https://code.google.com/p/google-diff-match-patch"
  s.description = "The Diff Match and Patch libraries offer robust algorithms to perform the operations required for synchronizing plain text."

  s.files = Dir.glob("ext/**/*.{c,rb}") +
  Dir.glob("lib/**/*.rb")

  s.extensions << "ext/diff_match_patch/extconf.rb"

  s.add_development_dependency "rake-compiler"
end
