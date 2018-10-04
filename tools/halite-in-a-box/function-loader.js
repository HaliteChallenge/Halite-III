module.exports = function(content) {
    this.cacheable();
    return this.query.fn(content);
};
