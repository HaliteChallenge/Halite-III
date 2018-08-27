export default class Camera {
    constructor(visualizer, initScale) {
        this.visualizer = visualizer;
        this.cols = visualizer.replay.production_map.width;
        this.rows = visualizer.replay.production_map.height;
        this.dragBase = [ 0, 0 ];
        this.dragging = false;
        this.mouseDown = false;

        this.initScale = initScale;
        this.scale = initScale;
        this.dirty = false;

        this.pan = { x: 0, y: 0 };
        this.pixelPan = { x: 0, y: 0 };

        this.selected = null;
    }

    reset() {
        this.scale = this.initScale;
        this.pan.x = 0;
        this.pan.y = 0;
        this.pixelPan.x = 0;
        this.pixelPan.y = 0;
        this.visualizer.panRender();
    }

    attach(view) {
        view.addEventListener("mousedown", this.onDragStart.bind(this));
        view.addEventListener("mousemove", this.onDragMove.bind(this));
        view.addEventListener("mouseleave", this.onDragStop.bind(this));
        view.addEventListener("mouseup", this.onDragStop.bind(this));
        view.addEventListener("wheel", this.onZoom.bind(this));
    }

    // Adjust coordinates to account for scaling on canvas
    scaledToScreen(x, y) {
        const canvas = this.visualizer.application.renderer.view;
        const rawZoom = canvas.style.zoom ? canvas.style.zoom : '1';
        let zoom = parseFloat(rawZoom);
        if (!Number.isFinite(zoom)) {
            zoom = 1;
        }
        return [ x / zoom, y / zoom ];
    }

    screenToWorld(x, y) {
        return [
            (Math.floor(x / this.scale) - this.pan.x + this.cols) % this.cols,
            (Math.floor(y / this.scale) - this.pan.y + this.rows) % this.rows,
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

        const sign = e.deltaY >= 0 ? -1 : 1;
        const delta = sign * Math.max(1, Math.min(2, Math.abs(e.deltaY) / 150));
        const percentX = (e.offsetX + 0.5 * this.scale) / this.visualizer.width;
        const percentY = (e.offsetY + 0.5 * this.scale) / this.visualizer.height;

        this.zoomBy(percentX, percentY, delta);
    }

    zoomBy(anchorX, anchorY, delta) {
        // Try to keep point under mouse fixed
        const [ centerX, centerY ] = this.screenToWorld(
            anchorX * this.visualizer.width,
            anchorY * this.visualizer.height
        );

        this.scale += delta;
        this.scale = Math.min(10 * this.initScale, Math.max(this.initScale, this.scale));

        const viewWidth = this.visualizer.width / this.scale;
        const viewHeight = this.visualizer.height / this.scale;
        const viewLeft = centerX - anchorX * viewWidth;
        const viewTop = centerY - anchorY * viewHeight;

        // 0.5 factor helps keeps things properly centered - otherwise
        // there's a bias towards the top-left corner
        this.pan.x = Math.round((-viewLeft - 0.5 + this.cols) % this.cols);
        this.pan.y = Math.round((-viewTop - 0.5 + this.rows) % this.rows);

        this.pixelPan.x = this.pan.x * this.scale;
        this.pixelPan.y = this.pan.y * this.scale;

        this.visualizer.panRender();
    }

    onDragStart(e) {
        if (e.which === 1) {
            this.dragBase = [ e.offsetX, e.offsetY ];
            this.mouseDown = true;
            this.dragging = false;
        }
    }

    onDragMove(e) {
        const dx = e.offsetX - this.dragBase[0];
        const dy = e.offsetY - this.dragBase[1];
        if (this.mouseDown && dx**2 + dy**2 > 25) {
            this.dragging = true;
        }

        if (this.dragging) {
            this.dragBase = [ e.offsetX, e.offsetY ];
            this.panByPixel(dx, dy);
        }
        else {
            const [ x, y ] = this.screenToWorld(
                ...this.scaledToScreen(e.offsetX, e.offsetY));
            this.visualizer.baseMap.hovered = { x, y };
            this.visualizer.panRender();
        }
    }

    panByPixel(dx, dy) {
        this.pixelPan.x += dx;
        this.pixelPan.y += dy;

        const fullWidth = this.scale * this.cols;
        const fullHeight = this.scale * this.rows;
        this.pixelPan.x = (this.pixelPan.x + fullWidth) % fullWidth;
        this.pixelPan.y = (this.pixelPan.y + fullHeight) % fullHeight;
        this.pan.x = Math.round(this.pixelPan.x / this.scale);
        this.pan.y = Math.round(this.pixelPan.y / this.scale);
        this.visualizer.panRender();
    }

    panBy(dx, dy) {
        this.pan.x += dx + this.cols;
        this.pan.y += dy + this.rows;
        this.pan.x %= this.cols;
        this.pan.y %= this.rows;

        this.pixelPan.x = this.pan.x * this.scale;
        this.pixelPan.y = this.pan.y * this.scale;

        this.visualizer.panRender();
    }

    onDragStop(e) {
        if (this.mouseDown && !this.dragging) {
            // Compute selection (mouse up and didn't drag)
            const [ worldX, worldY ] = this.screenToWorld(
                ...this.scaledToScreen(e.offsetX, e.offsetY));

            // TODO: add factory selection
            let selected = false;
            for (const ship of Object.values(this.visualizer.entity_dict)) {
                if (Math.floor(ship.x) === worldX &&
                    Math.floor(ship.y) === worldY) {
                    this.visualizer.onSelect.dispatch("ship", {
                        owner: ship.owner,
                        id: ship.id,
                    });
                    this.selected = {
                        type: "ship",
                        owner: ship.owner,
                        id: ship.id,
                    };
                    selected = true;
                }
            }
            if (!selected) {
                this.visualizer.onSelect.dispatch("point", {
                    x: worldX,
                    y: worldY,
                });
                this.selected = {
                    type: "point",
                    x: worldX,
                    y: worldY,
                };
            }
        }
        this.dragging = false;
        this.mouseDown = false;
        this.visualizer.baseMap.hovered = null;
        this.visualizer.panRender();
    }
}
