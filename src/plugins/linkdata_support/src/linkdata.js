if (typeof LinkData === "undefined" || !LinkData) {
    var LinkData = {};
}
if (typeof LinkData.app === "undefined" || !LinkData.app) {
    LinkData.app = {};
}

/**
 * LinkData.getWorks                                        ->    [<workId>,<workId>...]
 * LinkData.getFiles                                        ->    [{<workId>,[<fileName>,<fileName>...]},{.....}...]
 * LinkData.getSubjects(workId, filename)                    ->    [<subject>,<subject>...]
 * LinkData.getProperties(workId, filename)                    ->    [<property>,<property>,...]
 * LinkData.getObjects(workId, filename, subject, property)    ->    [<obejctValue>,<objectValue>,...]
 * LinkData.getTriples(workId, filename)                    ->    [{"subject":<subject>,"property":<property>, "object":<object>},...]
 * LinkData.getTriplesBySubject(workId, filename, subject)    ->    [{"subject":<subject>,"property":<property>, "object":<object>},...]
 * LinkData.getTriplesByProperty(workId, filename, property)->    [{"subject":<subject>,"property":<property>, "object":<object>},...]
 */

/**
 *
 * user accessing API functions
 *
 */

/**
 * @method LinkData.getSubjects
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @return {Array} The array of subject url values.
 *        ex.[<subject>,<subject>...]
 */
LinkData.getSubjects = function (workId, filename) {
    return LinkData.app.api.getSubjects(workId, filename);
}

/**
 * @method LinkData.getProperties
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @return {Array} The array of property url values.
 *        ex.[<property>,<property>,...]
 */
LinkData.getProperties = function (workId, filename) {
    return LinkData.app.api.getProperties(workId, filename);
}

/**
 * @method LinkData.getObjects
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @param {String} subject Url of subject
 * @param {String} property Url of property
 * @return {Array} The array of object values.
 *        ex.[<obejctValue>,<objectValue>,...]
 */
LinkData.getObjects = function (workId, filename, subject, property) {
    return LinkData.app.api.getObjects(workId, filename, subject, property);
}

/**
 * @method LinkData.getTriples
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @return {Array} The array of all triples.
 *        ex.[{"subject":<subject>,"property":<property>, "object":<object>},...]
 */
LinkData.getTriples = function (workId, filename) {
    return LinkData.app.api.getTriples(workId, filename);
}

/**
 * @method LinkData.getTriplesBySubject
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @param {String} subject Url of subject (optional). if null return triples of all properties
 * @return {Array} The array of triples with subject.
 *        ex.[{"subject":<subject>,"property":<property>, "object":<object>},...]
 */
LinkData.getTriplesBySubject = function (workId, filename, subject) {
    return LinkData.app.api.getTriplesBySubject(workId, filename, subject);
}

/**
 * @method LinkData.getTriplesByProperty
 * @static
 * @param {String} workId Id of rdf work
 * @param {String} filename Name of rdf work file
 * @param {String} property Url of property (optional). if null return triples of all properties
 * @return {Array} The array of triples with property.
 *        ex.[{"subject":<subject>,"property":<property>, "object":<object>},...]
 */
LinkData.getTriplesByProperty = function (workId, filename, property) {
    return LinkData.app.api.getTriplesByProperty(workId, filename, property);
}


/**
 *
 * @private
 * application internal API functions
 *
 * {{},{}...} case for (var obj in data) and [{},{}...] for (var i = 0, i < data.length, i++)
 *
 */
LinkData.app.api = {

    getSubjects: function (workId, filename) {
        var result = [];
        var json = this._getJson(workId, filename);

        for (var subject in json) {
            if (this._exist(subject)) {
                result.push(subject);
            }
        }

        return result;
    },

    getProperties: function (workId, filename) {
        var result = [];
        var json = this._getJson(workId, filename);

        var properties = [];
        for (var subject in json) {
            if (this._exist(subject)) {
                var propertyList = json[subject];
                for (var property in propertyList) {
                    if (this._exist(property) && this._arrContains(properties, property) < 0) {
                        properties.push(property);
                        result.push(property);
                    }
                }
            }
        }

        return result;
    },

    getObjects: function (workId, filename, subject, property) {
        var result = [];
        var json = this._getJson(workId, filename);

        for (var sub in json) {
            if (this._exist(sub) && sub == subject) {
                var propertyList = json[sub];
                for (var prop in propertyList) {
                    if (this._exist(prop) && prop == property) {
                        var objectList = propertyList[prop];
                        for (var i = 0; i < objectList.length; i++) {
                            result.push(objectList[i].value);
                        }
                    }
                }
            }
        }

        return result;
    },

    getTriples: function (workId, filename) {
        return this._getTriples(workId, filename, null, null);
    },

    getTriplesBySubject: function (workId, filename, subject) {
        return this._getTriples(workId, filename, subject, null);
    },

    getTriplesByProperty: function (workId, filename, property) {
        return this._getTriples(workId, filename, null, property);
    },

    _getTriples: function (workId, filename, subject, property) {
        var result = [];
        var json = this._getJson(workId, filename);

        for (var sub in json) {
            if (this._exist(sub) && this._valid(sub, subject)) {
                var propertyList = json[sub];
                for (var prop in propertyList) {
                    if (this._exist(prop) && this._valid(prop, property)) {
                        var objectList = propertyList[prop];
                        for (var i = 0; i < objectList.length; i++) {
                            var triple = new Object();
                            triple["subject"] = sub;
                            triple["property"] = prop;
                            triple["object"] = objectList[i].value;
                            result.push(triple);
                        }
                    }
                }
            }
        }

        return result;
    },

    _arrContains: function (arr, data) {
        for (var i = 0; i < arr.length; i++) {
            if (arr[i] === data) {
                return i;
            }
        }
        return -1;
    },

    _exist: function (data) {
        return (data && data.length > 0);
    },

    _valid: function (value, param) {
        if (param) {
            return (param == value);
        }
        return true;
    },

    _getJson: function (workId, filename) {
        if (!(LinkData.userdata[workId] && LinkData.userdata[workId][filename])) {
            var file = LinkData.userdata.fetchFile(workId, filename);
            if (LinkData.userdata[workId] === undefined) {
                LinkData.userdata[workId] = {};
            }
            LinkData.userdata[workId][filename] = file;
        }
        return LinkData.userdata[workId][filename];
    }
};

/**
 *
 * @private
 * linkdata traversing method by class name
 *
 */
LinkData.getElementsByClassName = function (classname, tagname, root, fn) {
    if (!root) {
        root = document.getElementsByTagName('body')[0];
    }
    var a = [];
    var re = new RegExp('\\b' + classname + '\\b');
    var els = root.getElementsByTagName(tagname);
    if (fn) {
        for (var i = 0, j = els.length; i < j; i++) {
            if (re.test(els[i].className)) {
                fn(els[i]);
            }
        }
    } else {
        for (var i = 0, j = els.length; i < j; i++) {
            if (re.test(els[i].className)) {
                a.push(els[i]);
            }
        }
    }
    return a;
};