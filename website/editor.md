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

<div class="container-fluid" role="main">
	<div class="row flex-xl-nowrap">
		<div class="col-12 col-md-1 col-xl-1 bd-sidebar">
			<nav id="bd-docs-nav"><div class="bd-toc-item active">
				<ul class="nav bd-sidenav"><li class="active bd-sidenav-active">
				<a href="">
				  File1
				</a></li><li>
				<a href="">
				  File2
				</a></li><li>
				<a href="https://getbootstrap.com/docs/4.1/getting-started/accessibility/">
				  File3
				</a></li></ul>
			</div></nav>
		</div>
		<div class="col-12 col-md-7 col-xl-7 py-md-7 pl-md-7 bd-content">
			<div id="bot-editor-container">
	    	</div>
	    </div>
		<div class="col-12 col-md-4 col-xl-4 py-md-4 pl-md-4 bd-toc">
		</div>
	</div>
</div>
