from . import app, setup_logging
from . import config
from . import editor

app.config["MAX_CONTENT_LENGTH"] = config.MAX_BOT_FILE_SIZE

setup_logging("editor_server.log", app.logger)
app.register_blueprint(editor.editor_api, url_prefix="/v1/editor")
