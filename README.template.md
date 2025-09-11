{% extends "./KoalaBox/templates/README.base.md" %}
{% block content %}

_Legit DLC Unlocker for Epic Online Services._

## ✨ Features

* `🔓` Emulate DLC ownership in legitimately owned games
* `🛅` Emulate Entitlements ownership
* `⌚` Block playtime tracking
* `✒️` Print EOS SDK logs

{% include "KoalaBox/templates/markdown/links.md" %}

{% include "KoalaBox/templates/markdown/intro.md" %}

{% include "KoalaBox/templates/markdown/usage.md" %}

{% include "KoalaBox/templates/markdown/installation.md" %}

> [!IMPORTANT]
> When a game opens a web page on {{ store_name }} website, which asks your permission to share your profile info
> with the game, you need to accept that for the DLC unlocking to work properly.
> Many games simply don't start DLC ownership checks until they have received this permission.

{% include "KoalaBox/templates/markdown/configuration.md" %}

[//]: # (Section about when ScreamAPI fetches entitlements)

## 🎓 Extra info

### When does ScreamAPI handle entitlement requests?

When a game requests a list of entitlements that user owns, it may specify an optional list of entitlement IDs. The {{ store_sdk }} is supposed to respond only with entitlements that match these IDs. Hence, {{ project_name }} can use this information to generate a response which contains all entitlements specified by the game. However, if a game does not provide a list of entitlement IDs then {{ project_name }} has to resort to making a request to {{ store_name }} Store servers to retrieve all offered DLCs, whose IDs will be used to generate a response.

{% include "KoalaBox/templates/markdown/troubleshooting.md" %}

{% include "KoalaBox/templates/markdown/building.md" %}

{% include "KoalaBox/templates/markdown/acknowledgements.md" %}

{% endblock %}