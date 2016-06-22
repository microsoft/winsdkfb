
function hide_class_elements(class_name) {
    var elements = document.getElementsByClassName(class_name);
    for (var i = 0; i < elements.length; ++i) {
        elements[i].parentNode.style.display = "none";
    }
}

function show_class_elements(class_name) {
    var elements = document.getElementsByClassName(class_name);
    for (var i = 0; i < elements.length; ++i) {
        elements[i].parentNode.style.display = "block";
    }
}

// saves cookie with language preference. cookie will expire in 1 year
function save_language_preference(value) {
    document.cookie = "language_preference=" + value + ";path=/;max-age=" + 31536e3 + ";";
}

function get_language_preference() {
    var cookies = document.cookie.split(";");
    var reg = /\s*language_preference=(\w*)[;\s]*/;
    for (var i = 0; i < cookies.length; ++i) {
        var arr = reg.exec(cookies[i]);
        if (arr !== null) {
            return arr[1];
        }
    }
    return null;
}

function run_language_pref_update(selected_value) {
    if (selected_value === "cpp") {
        hide_class_elements("language-csharp");
        show_class_elements("language-c--");
    }
    else if (selected_value === "c_sharp") {
        hide_class_elements("language-c--");
        show_class_elements("language-csharp");
    }
    else {
        console.log("unknown value for language preference: ", selected_value);
        return;
    }
    save_language_preference(selected_value);
}

function run_default_for_page_load() {
    var selection = get_language_preference();
    if (selection === null) {
        selection = "cpp";
    }
    run_language_pref_update(selection)
    // set radio buttons active on correct value
    var radios = document.language_select_form.lang_select;
    for (var i = 0; i < radios.length; ++i) {
        if (radios[i].value === selection) {
            radios[i].checked = true;
        }
    }
}