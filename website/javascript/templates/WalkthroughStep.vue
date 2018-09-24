<!-----------------
     A single step in a walkthrough.
   ------------------>
<template>
<section class="step" v-bind:class="{ active }" v-on:click="switchTo">
  <img class="pixel" src="/assets/images/tutorial-images/step-pixel.png" alt="pixel"/>
  <h2>
    {{title}}
    <img src="/assets/images/icons/arrow-right-grey.png" alt="more"/>
  </h2>
  <slot></slot>
</section>
</template>

<script>
export default {
  name: 'walkthrough-step',
  props: ['title', 'name'],
  data: function () {
    return {
      active: false,
      switchTo: () => {}
    }
  },
  computed: {}
}
</script>

<style lang="scss" scoped>
$primary-blue-text: #033C89;
.step {
  height: 3em;
  overflow: hidden;
  margin: 5px;
  margin-right: 0;
  background:#fff;
  border: none;
  border-left: 3px solid $primary-blue-text;
  padding: 17px 20px 17px 24px;
  cursor: pointer;
  >.pixel{
    width: calc(100% + 30px);
    margin-left: -20px;
    margin-top: -15px;
    display: none;
  }
  >h2 {
    margin: 0;
    margin-bottom: 10px;
    transition: 1s all ease-in-out;
    font-weight: 600;
    font-size: 16px;
    line-height: 19px;
    text-shadow: 0 1px 16px 0 #0098E3, 0 1px 8px 0 #00ABFF;
    img{
      width: 10px;
      float: right;

    }
  }
  >button{
    height: 38px;
    width: 128px;
    border-radius: 4px;
    background: linear-gradient(180deg, #FAD961 0%, #F76B1C 100%);
    border: none;
    color: #fff;
    &:focus{
      outline: none;
    }
    >img{
      width: 8px;
      margin-right: 5px;
    }
  }
}

.step.active {
  height: auto;
  >.pixel{
    display: block;
  }
  >h2 {
    img{
      display: none;
    }
  }
}

</style>
