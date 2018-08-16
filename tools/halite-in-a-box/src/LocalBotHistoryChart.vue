<template>
    <svg :width="width" :height="height" ref="svg">
        <g :transform="`translate(${margin.left}, ${margin.top})`">
            <path class="line"></path>
            <g class="axis-x" :transform="`translate(0, ${chartHeight})`"></g>
            <g class="axis-y"></g>
        </g>
    </svg>
</template>

<script>
    // Based on https://bl.ocks.org/d3noob/402dd382a51a4f6eea487f9a35566de0
    import * as d3 from 'd3';

    export default {
        props: ['width', 'height', 'data'],
        data() {
            return {
                margin: { top: 20, left: 30, bottom: 30, right: 0 },
            };
        },
        mounted() {
            this.draw(this.data);
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
            draw(rawData) {
                const x = d3.scaleTime().range([ 0, this.chartWidth ]);
                const y = d3.scaleLinear().range([ this.chartHeight, 0 ]);

                const line = d3.line()
                               .x(d => x(d.datetime))
                               .y(d => y(d.mu - 3*d.sigma));

                const svg = d3.select(this.$refs.svg);
                const data = rawData.map((d) => {
                    d.datetime = new Date(d.datetime);
                    return d;
                });

                x.domain(d3.extent(data, (d) => d.datetime));
                y.domain([0, d3.max(data, (d) => d.mu - 3*d.sigma)]);
                svg.select('path')
                   .data([ data ])
                   .attr('d', line);

                svg.select('.axis-x')
                   .call(d3.axisBottom(x));
                svg.select('.axis-y')
                   .call(d3.axisLeft(y));
            },
        },
        watch: {
            data(data) {
                this.draw(data);
            },
        }
    };
</script>

<style lang="scss" scoped>
    .line {
        fill: none;
        stroke: #000;
        stroke-width: 2;
    }
</style>
