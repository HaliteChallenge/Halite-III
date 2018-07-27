<template>
    <li class="bd-sidenav-active tree_files">
      <template v-if="typeof obj.contents === 'string'">
          <div class="tree_file_container">
            <a v-on:click="$emit('event_file_selected', obj)">
            {{ name }}
            </a>
          </div>
      </template>
      <template v-else>
        <div v-if="!is_root" class="tree_folder_container">
          <span v-on:click="toggle_visibility()"><span class="glyphicon" :class="{'glyphicon-chevron-right': !show_children, 'glyphicon-chevron-down': show_children}"></span></span>
          <span>{{ name }}</span>
        </div>
        <ul class="nav bd-sidenav" :class="{'indent': !is_root, 'hidden': !show_children}">
          <TreeElement
            v-for="(subobj, key) in obj" 
            :obj="subobj" 
            :name="key"
            :depth="depth + 1"
            @event_file_selected="pass_up">
          </TreeElement>
        </ul>
      </template>
    </li>
</template>
<script>
export default { 
  props: [ 'name', 'obj', 'depth' ],
  name: 'TreeElement',
  data: function() {
    let is_root = this.depth === -1
    return {
      show_children: is_root,
      is_root: is_root
    }
  },
  methods: {
    pass_up: function(obj) {
      console.log(obj)
      this.$emit('event_file_selected', obj)
    },
    toggle_visibility: function() {
      this.show_children = !this.show_children
    }
  },
  watch: {
    obj: function(new_value) {
      console.log(new_value)
    },
    show_children: function(new_value) {
      console.log(new_value)
    }
  }
}
</script>
<style lang="scss" scoped>

.tree_files {
  font-size: 9pt;
}

.tree_folder_container, .tree_file_container {
  padding-left: 15px;
  padding-top: 10px;
  padding-bottom: 10px;
  position: relative;
  display: block;
}

.tree_file_container {
  padding-left: 18px;
}

.tree_folder_container {
  padding-left: 5px;
}

.tree_folder_container .glyphicon {
  font-size: 10px;
}

.indent {
  margin-left: 15px;
}

</style>
