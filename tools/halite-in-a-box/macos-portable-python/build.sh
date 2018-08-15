if [[ ! -d venv ]]; then
    virtualenv -p python3 venv
    source venv/bin/activate
else
    source venv/bin/activate
fi

pip install -r requirements.txt
pip install -r ../../hlt_client/requirements.txt
python setup.py py2app
cd dist/
zip -r ../python-macos.zip python-macos.app
