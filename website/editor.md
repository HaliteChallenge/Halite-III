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
		background-image: none;
		background-color: #25242f;
	}
	.body {
		height: 100%;
	}
	body {
		height: calc(100% - 71px);
	}
	/* Track */
	::-webkit-scrollbar {
		border-left: 1px solid #424C53;
		background-color: #25242f; 
	}

		/* Handle */
	::-webkit-scrollbar-thumb {
		background-color: #26343F; 
		border-radius: 10px;
		border: 0.5px solid #30414f;
	}

		/* Handle on hover */
	::-webkit-scrollbar-thumb:hover {
		background-color: #26343F; 
	}
</style>
