---
layout: doc_page
title: Download
description: Halite engine and language specific starter kit downloads
image: assets/images/opengraph.png
content: website
sort_key: 3
---

Download a language specific starter kit as a quick and easy way to get started playing in the Halite AI competition. Can't find your preferred language? We love contributions! Check out our [guide](create-new-starter-kit) on how to build a new starter bot.

## System Requirements
System requirements are detailed [here](system-requirements).

## Starter kits Download

<div class="table-container">
    <table class="table">
        <thead>
            <tr>
                <td></td>
                <th colspan="{{ site.data.downloads.platforms | size }}" class="text-center">Operating System</th>
            </tr>
            <tr>
                <th>Language</th>
                <td>Version</td>
                {% for platform in site.data.downloads.platforms %}
                <td>{{ platform }}</td>
                {% endfor %}
            </tr>
        </thead>
        <tbody>
            {% for language in site.data.downloads.languages %}
            <tr>
                <td>{{ language.language }}</td>
                <td>{{ language.version }}</td>
                {% for file in language.files %}
                <td><a href="{{ site.baseurl }}/{{ file }}">Download</a></td>
                {% endfor %}
            </tr>
            {% endfor %}
            <tr>
                <td>Only Game Environment</td>
                <td>{{ site.data.downloads.version }}</td>
                <td>NA</td>
                {% for file in site.data.downloads.environments %}
                <td><a href="{{ site.baseurl }}/{{ file }}">Download</a></td>
                {% endfor %}
            </tr>
        </tbody>
    </table>
</div>

## Halite Tools and Source Download

<div class="table-container">
    <table class="table">
        <thead>
            <tr>
                <th>Tool</th>
                <td>Version</td>
                <td>All Platforms</td>
                <td>MacOS</td>
                <td>Linux</td>
                <td>Windows</td>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>Source</td>
                <td>NA</td>
                <td><a href="{{ site.baseurl }}/{{ site.data.downloads.source }}">Download</a></td>
                <td>NA</td>
                <td>NA</td>
                <td>NA</td>
            </tr>
        </tbody>
    </table>
</div>
