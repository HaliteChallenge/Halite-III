---
layout: base_new
stoc: false
full_width: true
permalink: /editor
menu: editor
mount_views: ["BotEditorPage"]
additional_js: ["editorBuild/code_edit/built-codeEdit.min"]
additional_css: ["built-codeEdit", "dark-codeEdit"]
---

<div id="bot-editor-container">
</div>

<style>
	html {
		height: 100%;
		background-image: none;
		background-color: #25242f;
	}
	.body {
		height: 100%;
	}
	body {
		height: calc(100% - 71px);
	}
	::-webkit-scrollbar {
		width: 8px;
		background-color: #25242f;
	}
	::-webkit-scrollbar-thumb {
		background-color: #26343F;
		border-radius: 3px;
		width: 100%;
		border: 0.5px solid #30414f;
	}
</style>
