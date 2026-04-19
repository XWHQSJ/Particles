# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| master  | Yes       |

## Reporting a Vulnerability

If you discover a security vulnerability in this project, please report it responsibly.

**Email:** Open a GitHub issue with the label `security` or contact the maintainer directly.

Since this is a graphics demo project with no network services, server components, or user data handling, the attack surface is limited to:

- Buffer overflows in particle/texture processing
- Malformed image file handling (via stb_image)
- Shader injection (mitigated by embedded default shaders)

## Response

- Acknowledged within 7 days
- Fix released within 30 days for confirmed issues
- Credit given to reporter unless anonymity is requested
