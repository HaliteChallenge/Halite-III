import * as assets from "./assets";

export default class Camera {
    constructor(initScale, container, cols, rows) {
        this.cols = cols;
        this.rows = rows;
        this.container = container;
        this.dragBase = [ 0, 0 ];
        this.dragging = false;
        this.mouseDown = false;

        this.initScale = initScale;
        this.scale = initScale;
        this.dirty = false;
    }

    attach(view) {
        view.addEventListener("mousedown", this.onDragStart.bind(this));
        view.addEventListener("mousemove", this.onDragMove.bind(this));
        view.addEventListener("mouseleave", this.onDragStop.bind(this));
        view.addEventListener("mouseup", this.onDragStop.bind(this));
        view.addEventListener("mousewheel", this.onZoom.bind(this));
    }

    screenToWorld(x, y) {
        return [
            Math.floor((x - this.container.position.x) / this.scale),
            Math.floor((y - this.container.position.y) / this.scale),
        ];
    }

    update() {
    }

    onZoom(e) {
        e.preventDefault();

        const centerX = e.offsetX;
        const centerY = e.offsetY;
        const sign = e.wheelDelta >= 0 ? 1 : -1;
        const delta = sign * Math.max(1, Math.min(2, Math.abs(e.wheelDelta) / 150));

        this.scale += delta;
        this.scale = Math.min(15, Math.max(1, this.scale));
        this.dirty = true;
    }

    onDragStart(e) {
        if (e.which === 1) {
            this.dragBase = [ e.offsetX, e.offsetY ];
            this.mouseDown = true;
        }
    }

    onDragMove(e) {
        const dx = e.offsetX - this.dragBase[0];
        const dy = e.offsetY - this.dragBase[1];
        if (this.mouseDown && dx**2 + dy**2 > 25) {
            this.dragging = true;
        }

        if (this.dragging) {
            this.container.position.x += dx;
            this.container.position.y += dy;

            // Constrain translation
            const fullWidth = this.scale * this.cols;
            const fullHeight = this.scale * this.rows;
            this.container.position.y = Math.max(assets.VISUALIZER_HEIGHT - fullHeight,
                                                 this.container.position.y);
            this.container.position.y = Math.min(0, this.container.position.y);
            this.container.position.x = Math.max(assets.VISUALIZER_SIZE - fullWidth,
                                                 this.container.position.x);
            this.container.position.x = Math.min(0, this.container.position.x);
            this.dragBase = [ e.offsetX, e.offsetY ];
        }
    }

    onDragStop(e) {
        this.dragging = false;
        this.mouseDown = false;
    }
}
