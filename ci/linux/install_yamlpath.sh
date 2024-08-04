# Any commands which fail will cause the shell script to exit immediately
set -e

# Install yamlpath.
# https://github.com/wwkimball/yamlpath
# https://pypi.org/project/yamlpath/

echo Validate if python launcher is installed
which py
echo Found python interpreter
python --version
echo

echo Validate if pip is installed
which pip
echo

# Installing yamlpath.
echo Proceeding with instaling yamlpath...
pip install yamlpath
echo

echo yamlpath was installed on the system without error.
echo.
