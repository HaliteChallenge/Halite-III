<template>
    <svg :width="width" :height="height" ref="svg">
        <g :transform="`translate(${margin.left}, ${margin.top})`">
            <path v-for="data in lines" class="line"></path>
            <g class="axis-x" :transform="`translate(0, ${chartHeight})`"></g>
            <g class="axis-y"></g>
        </g>
    </svg>
</template>

<script>
    // Based on https://bl.ocks.org/d3noob/402dd382a51a4f6eea487f9a35566de0
    import * as d3 from 'd3';

    export default {
        props: ['width', 'height', 'lines', 'invertY', 'startYZero'],
        data() {
            return {
                margin: { top: 20, left: 30, bottom: 30, right: 0 },
            };
        },
        mounted() {
            this.draw();
        },
        computed: {
            chartWidth() {
                return this.width - (this.margin.left + this.margin.right);
            },
            chartHeight() {
                return this.height - (this.margin.top + this.margin.bottom);
            },
        },
        methods: {
            draw() {
                const x = d3.scaleLinear().range([ 0, this.chartWidth ]);
                const y = d3.scaleLinear().range(this.invertY ?
                                                 [ 0, this.chartHeight ] :
                                                 [ this.chartHeight, 0 ]);

                const svg = d3.select(this.$refs.svg);

                let dataMin = 2;
                let dataMax = 1;
                let allData = [];
                let dataLists = [];
                for (let i = 0; i < this.lines.length; i++) {
                    const [ rawData, yFunc ] = this.lines[i];

                    const data = rawData.map((d) => {
                        d.datetime = new Date(d.datetime);
                        return d;
                    });
                    data.sort((a, b) => d3.ascending(a.datetime, b.datetime));
                    data.forEach((d, i) => {
                        d.gamesPlayed = i;
                    });
                    dataLists.push(data);
                    allData = allData.concat(data);
                    dataMax = Math.max(dataMax, d3.max(data, yFunc));
                    dataMin = Math.min(dataMin, d3.min(data, yFunc));
                }
                x.domain(d3.extent(allData, (d) => d.gamesPlayed));
                y.domain([this.startYZero ? 0 : dataMin, dataMax]);

                for (let i = 0; i < this.lines.length; i++) {
                    const [ _, yFunc ] = this.lines[i];
                    const line = d3.line()
                                   .x(d => x(d.gamesPlayed))
                                   .y(d => y(yFunc(d)));

                    svg.select(`path:nth-child(${i + 1})`)
                       .data([ dataLists[i] ])
                       .attr('d', line);
                }


                svg.select('.axis-x')
                   .call(d3.axisBottom(x));
                svg.select('.axis-y')
                   .call(d3.axisLeft(y));
            },
        },
        watch: {
            lines() {
                this.draw();
            },
        },
    };
</script>

<style lang="scss" scoped>
    .line {
        fill: none;
        stroke: #000;
        stroke-width: 2;
    }
</style>
