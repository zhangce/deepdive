FILE(REMOVE_RECURSE
  "CMakeFiles/PrepareTest"
  "/tmp/deepdive"
  "/tmp/deepdive/variables.tsv"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/PrepareTest.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
