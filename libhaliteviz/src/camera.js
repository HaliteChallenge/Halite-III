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

        this.pan = { x: 0, y: 0 };
        this.pixelPan = { x: 0, y: 0 };
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

    worldToCamera(cellX, cellY) {
        return [
            (cellX + this.pan.x + this.cols) % this.cols,
            (cellY + this.pan.y + this.rows) % this.rows,
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
        this.scale = Math.min(32, Math.max(this.initScale, this.scale));
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
            this.pixelPan.x += dx;
            this.pixelPan.y += dy;

            const fullWidth = this.scale * this.cols;
            const fullHeight = this.scale * this.rows;
            this.dragBase = [ e.offsetX, e.offsetY ];
            this.pixelPan.x = (this.pixelPan.x + fullWidth) % fullWidth;
            this.pixelPan.y = (this.pixelPan.y + fullHeight) % fullHeight;
            this.pan.x = Math.round(this.pixelPan.x / this.scale);
            this.pan.y = Math.round(this.pixelPan.y / this.scale);
        }
    }

    onDragStop(e) {
        this.dragging = false;
        this.mouseDown = false;
    }
}
