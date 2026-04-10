var fso = new ActiveXObject("Scripting.FileSystemObject");

function fail(message) {
    WScript.StdErr.WriteLine(message);
    WScript.Quit(1);
}

function readUtf8(path) {
    var stream = new ActiveXObject("ADODB.Stream");
    stream.Type = 2;
    stream.Charset = "utf-8";
    stream.Open();
    stream.LoadFromFile(path);
    var text = stream.ReadText();
    stream.Close();
    return text;
}

function writeUtf8(path, text) {
    var stream = new ActiveXObject("ADODB.Stream");
    stream.Type = 2;
    stream.Charset = "utf-8";
    stream.Open();
    stream.WriteText(text);
    stream.SaveToFile(path, 2);
    stream.Close();
}

function repeatText(text, count) {
    var result = "";
    for (var index = 0; index < count; index += 1) {
        result += text;
    }
    return result;
}

function escapeString(value) {
    return value
        .replace(/\\/g, "\\\\")
        .replace(/"/g, "\\\"")
        .replace(/\r/g, "\\r")
        .replace(/\n/g, "\\n")
        .replace(/\t/g, "\\t");
}

function isArray(value) {
    return Object.prototype.toString.call(value) === "[object Array]";
}

function stringify(value, depth) {
    var indent = repeatText("\t", depth);
    var childIndent = repeatText("\t", depth + 1);
    var items;
    var key;
    var index;

    if (value === null) {
        return "null";
    }

    switch (typeof value) {
        case "string":
            return "\"" + escapeString(value) + "\"";
        case "number":
        case "boolean":
            return String(value);
        case "object":
            if (isArray(value)) {
                if (value.length === 0) {
                    return "[]";
                }

                items = [];
                for (index = 0; index < value.length; index += 1) {
                    items.push(childIndent + stringify(value[index], depth + 1));
                }
                return "[\n" + items.join(",\n") + "\n" + indent + "]";
            }

            items = [];
            for (key in value) {
                if (value.hasOwnProperty(key)) {
                    items.push(childIndent + "\"" + escapeString(key) + "\": " + stringify(value[key], depth + 1));
                }
            }

            if (items.length === 0) {
                return "{}";
            }

            return "{\n" + items.join(",\n") + "\n" + indent + "}";
        default:
            fail("Unsupported JSON value type while writing .uproject: " + (typeof value));
    }
}

if (WScript.Arguments.length < 2) {
    fail("Usage: cscript install-plugin.js <TargetProject.uproject> <PluginName>");
}

var uprojectPath = WScript.Arguments(0);
var pluginName = WScript.Arguments(1);

if (!fso.FileExists(uprojectPath)) {
    fail("Missing .uproject file: " + uprojectPath);
}

var rawText = readUtf8(uprojectPath).replace(/^\uFEFF/, "");
var projectDescriptor;

try {
    projectDescriptor = eval("(" + rawText + ")");
} catch (error) {
    fail("Failed to parse .uproject JSON: " + error.message);
}

if (!projectDescriptor.Plugins || !isArray(projectDescriptor.Plugins)) {
    projectDescriptor.Plugins = [];
}

var existingEntry = null;
for (var pluginIndex = 0; pluginIndex < projectDescriptor.Plugins.length; pluginIndex += 1) {
    if (projectDescriptor.Plugins[pluginIndex] && projectDescriptor.Plugins[pluginIndex].Name === pluginName) {
        existingEntry = projectDescriptor.Plugins[pluginIndex];
        break;
    }
}

if (!existingEntry) {
    existingEntry = {
        Name: pluginName,
        Enabled: true,
        TargetAllowList: ["Editor"]
    };
    projectDescriptor.Plugins.push(existingEntry);
} else {
    existingEntry.Enabled = true;
    existingEntry.TargetAllowList = ["Editor"];
}

writeUtf8(uprojectPath, stringify(projectDescriptor, 0) + "\n");
WScript.StdOut.WriteLine("Updated " + uprojectPath);
