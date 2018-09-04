<!-----------------
     Resuable component to provide a sidebar with a list of steps &
     explanatory text (and possibly substeps), while indicating this
     information to a wrapped component.
   ------------------>
<script>
export default {
  name: 'walkthrough',
  props: ['title'],
  data: function () {
    return {
      progress: 0,
      steps: [],
      substeps: {}
    }
  },
  provide () {
    return {
      substeps: this.substeps,
      completeSubstep: (substep) => {
        this.$set(this.substeps, substep, true)
      }
    }
  },
  created: function () {
    // let i = 0
    for (const node of this.$slots.steps) {
      if (node.tag && node.tag.endsWith('walkthrough-step')) {
        // i += 1;
        const step = Object.assign({}, node.componentOptions.propsData)
        step.body = node.componentOptions.children
        this.steps.push(step)
      }
    }
  },
  mounted: function () {
    document.body.classList.add('walkthrough-body')

    if (window.history.state && window.history.state.progress) {
      this.switchTo(window.history.state.progress, false)
    } else {
      this.switchTo(0, false)
    }
    // Make sure we overwrite data once in render func
    this.$forceUpdate()

    window.addEventListener('popstate', (e) => {
      if (e.state && typeof e.state.progress !== 'undefined') {
        this.switchTo(e.state.progress, false)
      }
    })
  },
  render: function (h) {
    const steps = this.$slots.steps
      .filter((node) => node.tag && node.tag.endsWith('walkthrough-step'))
    return h('section', {
      class: 'walkthrough'
    }, [
      h('section', {
        class: 'walkthrough-nav'
      }, [
        h('h1', this.title),
        h('section', {
          class: 'steps',
          ref: 'stepsContainer'
        }, steps.map((step, index) => {
          if (step.child) {
            step.child.active = index === this.progress
            step.child.switchTo = () => this.switchTo(index)
          }
          return step
        })),
        h('nav', [
          h('a', {
            attrs: {
              disabled: this.progress === 0
            },
            on: {
              click: this.prevStep
            }
          }, 'Back'),
          h('a', {
            attrs: {
              disabled: this.progress === this.steps.length - 1
            },
            on: {
              click: this.nextStep
            }
          }, 'Next')
        ])
      ]),

      h('section', {
        class: 'walkthrough-content'
      }, [
        this.$scopedSlots.content({
          progress: this.progress,
          stepName: this.steps[this.progress].name
        })
      ])
    ])
  },
  methods: {
    switchTo: function (index, addState = true) {
      this.progress = Math.min(this.steps.length - 1, Math.max(0, index))
      if (addState) {
        window.history.pushState({
          progress: this.progress
        }, this.steps[this.progress], `#tutorial-${this.progress}`)
      }

      const container = this.$refs.stepsContainer
      // TODO: don't hardcode 40 (=2.5em)
      container.scrollTop = 40 * this.progress
    },
    nextStep: function () {
      this.switchTo(this.progress + 1)
    },
    prevStep: function () {
      this.switchTo(this.progress - 1)
    }
  }
}
</script>

<style lang="scss" scoped>
$backgroud-grey: #F3F3F3;
$primary-blue-text: #033C89;
.walkthrough {
  display: flex;
  >.walkthrough-nav {
    display: flex;
    flex-direction: column;
    flex: 1 0;
    border-right: 1px solid #474951;
    min-width: 20em;
    max-width: 25em;
    background: $backgroud-grey;
    color: $primary-blue-text;
    >h1 {
      margin: 0;
      padding: 32px 30px 28px;
      flex: 0 0;
      background: #fff;
      font-size: 24px;	font-weight: 500;	line-height: 29px;
      text-shadow: 0 1px 16px 0 #0098E3, 0 1px 8px 0 #00ABFF;
    }
    >nav {
      flex-direction: row;
      padding: 25px 0;
      text-align: center;
      font-size: 16px;
      font-weight: 600px;
      line-height: 19px;
      a{
        display: inline-block;
        &:hover{
          text-decoration: none;
        }
        &:first-child{
          padding-right:25px;
          border-right: 1px solid #0E458F;
          &:before{
            content: '<';
            margin-right: 10px;
          }
        }
        &:last-child{
          padding-left: 25px;
          &:after{
            content:'>';
            margin-left: 10px;
          }
        }
      }
    }
    >.steps {
      flex: 1 0;
      list-style-type: none;
      overflow-y: scroll;
      margin: 0;
      padding: 0;
    }
  }
  >.walkthrough-content {
    flex: 2 0;
  }
}
</style>
