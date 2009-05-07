# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (CTEST_NIGHTLY_START_TIME "23:00:00 EDT")
SET (CTEST_PROJECT_NAME "LoXiM")

# Dart server to submit results (used by client)
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE_CDASH TRUE)
SET (CTEST_DROP_SITE "jloxim.mimuw.edu.pl/cdash")
SET (CTEST_DROP_LOCATION "/submit.php?project=LoXiM")

SET (CTEST_TRIGGER_SITE 
       "http://${CTEST_DROP_SITE}/cgi-bin/Submit-vtk-TestingResults.pl")

