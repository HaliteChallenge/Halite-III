export default class Camera {
    constructor(container, map) {
        this.container = container;
        this.map = map;
        this.dragBase = [ 0, 0 ];
        this.dragging = false;
        this.mouseDown = false;

        this.scale = 1.0;
        this.dirty = false;
    }

    attach(view) {
        view.addEventListener("mousedown", this.onDragStart.bind(this));
        view.addEventListener("mousemove", this.onDragMove.bind(this));
        view.addEventListener("mouseexit", this.onDragStop.bind(this));
        view.addEventListener("mouseup", this.onDragStop.bind(this));
        view.addEventListener("mousewheel", this.onZoom.bind(this));
    }

    update() {
        if (this.dirty) {
            this.map.regenerateBaseMap();
        }
        this.dirty = false;
    }

    onZoom(e) {
        e.preventDefault();

        const centerX = e.offsetX;
        const centerY = e.offsetY;
        const sign = e.wheelDelta >= 0 ? 1 : -1;
        const delta = sign * Math.max(1, Math.min(2, Math.abs(e.wheelDelta) / 150));

        this.scale += delta;
        this.scale = Math.min(15, Math.max(1, this.scale));
        this.map.scale = this.scale;
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
            this.container.position.x = dx;
            this.container.position.y = dy;
        }
    }

    onDragStop(e) {
        this.dragging = false;
        this.mouseDown = false;
    }
}
