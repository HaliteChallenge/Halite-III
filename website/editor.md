---
layout: base_new
no_header: editor
stoc: false
full_width: true
permalink: /editor
menu: editor
mount_views: ["BotEditor"]
additional_js: ["FileSaver.min", "jszip.min", "jszip-utils.min", "editorBuild/code_edit/built-codeEdit.min"]
additional_css: ["built-codeEdit", "dark-codeEdit"]
---

<div id="bot-editor-container">
</div>

<style>
	html {
		height: 100%;
	}
	.body {
		height: 100%;
	}
	body {
		height: calc(100% - 71px);
	}
</style>
