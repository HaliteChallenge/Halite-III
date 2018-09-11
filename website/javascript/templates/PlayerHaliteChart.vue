<template>
  <div class="player-halite-chart">
    <svg width='100%' height='147' ref="mainSvg"></svg>
  </div>
</template>
<script>
import Vue from 'vue'
import * as d3 from 'd3'
export default {
  name: 'PlayerHaliteChart',
  props: {
    chartData: {
      type: Array,
      // required: true
    },
    index: {
      type: Number,
      required: true
    },
    maxY: {
      type: Number,
      required: true
    },
    // maxLength: {
    //   type: Number,
    //   required: false
    // },
    // showChart: {
    //   type: Boolean,
    //   required: true
    // },
    // selectedPlayers: {
    //   type: Array,
    //   required: true
    // }
  },
  data() {
    return {
      innerSvg: null,
      dragLine: null,
      x: null,
      path1: null,
      path2: null,
      maxLength: 50
    }
  },
  methods: {
    initChart(options) {
      console.log('init chart')
      let {chartData} = options
      if (!chartData || !chartData.length) return
      const svg = d3.select(this.$refs.mainSvg).attr('class', 'main-svg')
      const svgPosition = svg.node().getBoundingClientRect()
      const margin = {top: 20, right: 20, bottom: 10, left: 50}
      const width = +svgPosition.width - margin.left - margin.right
      const height = +svgPosition.height - margin.top - margin.bottom
      if (width < 0) return
      const g1 = svg.append('g').attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')
      const g2 = svg.append('g').attr('transform', 'translate(' + margin.left + ',' + margin.top + ')').attr('class', 'group-2')
      const innerSvg = g1.append('foreignObject').attr('height', height).attr('width', width).append('svg')

      this.path1List = []
      this.path2List = []

      let x = d3.scaleLinear()
          .rangeRound([0, width])
      let y = d3.scaleLinear()
          .rangeRound([height, 0])
      let area = d3.area()
        .x(function (d) { return x(d.x) })
        .y1(function (d) { return y(d.y) })

      let line = d3.area()
        .x(function (d) { return x(d.x) })
        .y(function (d) { return y(d.y) })
      let dataSet = chartData
      // x.domain(d3.extent(getDataPeriod(dataSet, this.maxLength, this.index), function (d) { return d.x }))
      x.domain(d3.extent(dataSet, function (d) { return d.x }))

      let _dataSet = [].concat(dataSet)

      y.domain([0, this.maxY])
      area.y0(y(0))
      let yAxis = g1.append('g')
          .call(d3.axisLeft(y).ticks(5, 's'))
        yAxis.selectAll('.domain').attr('stroke', 'white')
        yAxis.selectAll('line').attr('stroke', '#000').attr('x2', width).attr('opacity', 0.2)
        yAxis.selectAll('.tick text').attr('fill', '#000').attr('font-size', '1.5rem')
        yAxis.selectAll('.domain')
        yAxis.select('.domain')
          .remove()
      this.initDragPositon = x(this.index)
      innerSvg.attr('width', this.initDragPositon)
          .attr('height', height)

      let path1 = g1.append('path')
      path1.datum(dataSet)
        .attr('stroke', '#15BBE8')
        .attr('fill', 'transparent')
        .attr('stroke-linejoin', 'round')
        .attr('stroke-dasharray', '1 10')
        .attr('stroke-dashunits', 'pathLength')
        .attr('stroke-linecap', 'round')
        .attr('stroke-width', 2)
        .attr('d', line)
        .attr('class', 'line-chart')
      let path2 = innerSvg.append('path')
      path2.datum(dataSet)
        .attr('fill', '#D9ECFD')
        .attr('fill-opacity', 1)
        .attr('stroke', '#D9ECFD')
        .attr('stroke-linejoin', 'round')
        .attr('stroke-linecap', 'round')
        .attr('stroke-width', 3)
        .attr('d', area)
      let linearGradient = path2.append('linearGradient')
      linearGradient.attr('x1', '0%').attr('y1', '0%').attr('x2', '0%').attr('y2', '100%')
      linearGradient.append('stop')
        .attr('offset', '0%')
        .attr('style','stop-color:rgb(217,236,253);stop-opacity:0')
      linearGradient.append('stop')
        .attr('offset', '0%')
        .attr('style','stop-color:rgb(217,236,253);stop-opacity:1')

      this.path1 = path1
      this.path2 = path2
      this.innerSvg = innerSvg
      this.x = x
      this.y = y
      this.area = area
      this.line = line
    }
  },
  mounted() {
    this.initChart({chartData: this.chartData})
  },
  watch: {
    chartData(chartData) {
      this.initChart({chartData})
    },
    index(index) {
      if(!this.path1 || !this.path2 || !this.innerSvg) return
      let dataSet = this.chartData
      this.x.domain(d3.extent(dataSet, function (d) { return d.x }))
      let _peroidData = dataSet
      this.path1.datum(_peroidData).attr('d', this.line)
      this.path2.datum(_peroidData).attr('d', this.area)
      this.innerSvg
        .transition()
        .ease(d3.easeLinear)
        .attr('width', this.x(index))
    }
  }
}
</script>
