namespace mach::http
{
	enum class StatusCode {
		OK = 200,
		Created = 201,
		Accepted = 202,
		NonAuthoritativeInformation = 203,
		NoContent = 204,
		PartialContent = 206,

		MovedPermanently = 301,
		MovedTemporarily = 302,
		TemporaryRedirect = 307,
		PermanentRedirect = 308,

		BadRequest = 400,
		Unauthorized = 401,
		Forbidden = 403,
		NotFound = 404,
		MethodNotAllowed = 405,
		RequestTimeout = 408,
		Conflict = 409,

		InternalServerError = 500
	};
}
