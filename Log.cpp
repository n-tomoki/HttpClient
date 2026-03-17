#include "pch.h"
#include "HttpClient.h"
#include "Log.h"
#include "winhttp.h"


CLog::CLog()
{
	m_bUse    = FALSE;
	m_bActive = FALSE;
}


CLog::~CLog()
{
	if (m_bActive) {
		Close();
	}
}


void CLog::Init()
{
	m_bUse = App.GetParamFileBOOL(L"Param", L"Log", FALSE);
	App.WriteParamFileBOOL(L"Param", L"Log", m_bUse);
}


BOOL CLog::Open(BOOL bNew)
{
	Init();

	if (!m_bUse)   { return FALSE; }
	if (m_bActive) { return TRUE; }

	m_dtToday.Now();

	m_strFileName.Format(L"API_SendReceive_%d.log", m_dtToday.GetYMD());

	m_bActive = fp.Open(m_strFileName, File::_APPEND, File::_UTF8);
	if (!bNew) { Puts(L"***<< 送信開始 >>***"); }

	return m_bActive;
}


void CLog::Close(BOOL bNew)
{
	if (!m_bUse)   { return; }

	if (m_bActive) {
		if (!bNew) { Puts(L"***<< 送信終了 >>***\r\n"); }
		fp.Close();
		fpThread.Close();
		m_bActive = FALSE;
	}
}

void CLog::Puts(const WCHAR *psz)
{
	if (!m_bUse)   { return; }

	m_rSemaphore.Lock();
	{
		CDay day;

		if (day.m_Date != m_dtToday) {
			Close(TRUE);
			Open(TRUE);
		}

		CString str;

		str.Format(L"[%08d %06d],%s",
			day.GetDateYMD(),
			day.GetTimeHMS(),
			psz);
		fp.Puts(str);
	}
	m_rSemaphore.Unlock();
}


void CLog::PutsErr(const WCHAR *psz)
{
	CString str;

	str.Format(L"%s:%s", psz, GetLastErrMsg());

	Puts(str);
}


const WCHAR *CLog::GetLastErrMsg()
{
	const WCHAR *psz = L"不明";

	DWORD dwCode = GetLastError();

	switch (dwCode) {
	case ERROR_WINHTTP_OUT_OF_HANDLES:
		psz = L"ハンドルが不足している、または利用可能なハンドルがない。";
		break;
	case ERROR_WINHTTP_TIMEOUT:
		psz = L"要求がタイムアウトしました。";
		break;
	case ERROR_WINHTTP_INTERNAL_ERROR:
		psz = L"内部エラーが発生しました。";
		break;
	case ERROR_WINHTTP_INVALID_URL:
		psz = L"URL が無効です。";
		break;
	case ERROR_WINHTTP_UNRECOGNIZED_SCHEME:
		psz = L"URL で、\"http:\" または \"https:\" 以外のスキームが指定されました。";
		break;
	case ERROR_WINHTTP_NAME_NOT_RESOLVED:
		psz = L"サーバー名を解決できません。";
		break;
	case ERROR_WINHTTP_INVALID_OPTION:
		psz = L"WinHttpQueryOption または WinHttpSetOption に対する要求で無効なオプション値が指定されました。";
		break;
	case ERROR_WINHTTP_OPTION_NOT_SETTABLE:
		psz = L"要求されたオプションを設定できず、クエリのみを実行します。";
		break;
	case ERROR_WINHTTP_SHUTDOWN:
		psz = L"WinHTTP 関数のサポートがシャットダウンまたはアンロードされています。";
		break;

	case ERROR_WINHTTP_LOGIN_FAILURE:
		psz = L"ログイン試行が失敗しました。 このエラーが発生した場合、要求ハンドルは WinHttpCloseHandle で閉じる必要があります。 最初にこのエラーを生成した関数を再試行する前に、新しい要求ハンドルを作成する必要があります。";
		break;
	case ERROR_WINHTTP_OPERATION_CANCELLED:
		psz = L"通常、操作が完了する前に要求が操作されていたハンドルが閉じられたため、操作は取り消されました。";
		break;
	case ERROR_WINHTTP_INCORRECT_HANDLE_TYPE:
		psz = L"指定されたハンドルの種類がこの操作に対して正しくありません。";
		break;
	case ERROR_WINHTTP_INCORRECT_HANDLE_STATE:
		psz = L"指定されたハンドルが正しい状態でないため、要求された操作を実行できません。";
		break;
	case ERROR_WINHTTP_CANNOT_CONNECT:
		psz = L"サーバーへの接続に失敗した場合に返されます。";
		break;
	case ERROR_WINHTTP_CONNECTION_ERROR:
		psz = L"サーバーとの接続がリセットまたは終了されたか、互換性のない SSL プロトコルが検出されました。";
		break;
	case ERROR_WINHTTP_RESEND_REQUEST:
		psz = L"WinHTTP 関数が失敗しました。 目的の関数は、同じ要求ハンドルで再試行できます。";
		break;
 
	case ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED:
		psz = L"サーバーがクライアント認証を要求したときに WinHttpReceiveResponse によって返されます。";
		break;

	// WinHttpRequest Component errors
	case ERROR_WINHTTP_CANNOT_CALL_BEFORE_OPEN:
		psz = L"Open メソッドを呼び出す前に要求された操作を実行できない場合は、HttpRequest オブジェクトによって返されます。";
		break;
	case ERROR_WINHTTP_CANNOT_CALL_BEFORE_SEND:
		psz = L"Send メソッドを呼び出す前に要求された操作を実行できない場合は、HttpRequest オブジェクトによって返されます。";
		break;
	case ERROR_WINHTTP_CANNOT_CALL_AFTER_SEND:
		psz = L"Send メソッドの呼び出し後に要求された操作を実行できない場合は、HttpRequest オブジェクトによって返されます。";
		break;
	case ERROR_WINHTTP_CANNOT_CALL_AFTER_OPEN:
		psz = L"Open メソッドの呼び出し後に指定したオプションを要求できない場合は、HttpRequest オブジェクトによって返されます。";
		break;

	// HTTP API errors
	case ERROR_WINHTTP_HEADER_NOT_FOUND:
		psz = L"要求されたヘッダーを見つけられない。";
		break;
	case ERROR_WINHTTP_INVALID_SERVER_RESPONSE:
		psz = L"サーバーの応答を解析できません。";
		break;
	case ERROR_WINHTTP_INVALID_HEADER:
		psz = L"HTTPヘッダーに問題があります。";
		break;
	case ERROR_WINHTTP_INVALID_QUERY_REQUEST:
		psz = L"HTTP リクエストに無効なクエリリクエストが発生した。";
		break;
	case ERROR_WINHTTP_HEADER_ALREADY_EXISTS:
		psz = L"指定したヘッダーは既に存在している。";
		break;
	case ERROR_WINHTTP_REDIRECT_FAILED:
		psz = L"スキームが変更されたか、すべてのリダイレクト試行が失敗したため、リダイレクトに失敗しました (既定値は 5 回です)。";
		break;

	// additional WinHttp API error codes
	case ERROR_WINHTTP_AUTO_PROXY_SERVICE_ERROR:
		psz = L"指定した URL のプロキシが見つからない場合に WinHttpGetProxyForUrl によって返されます。";
		break;
	case ERROR_WINHTTP_BAD_AUTO_PROXY_SCRIPT:
		psz = L"プロキシ自動構成 (PAC) ファイルでスクリプト コードを実行中にエラーが発生しました。";
		break;
	case ERROR_WINHTTP_UNABLE_TO_DOWNLOAD_SCRIPT:
		psz = L"PAC ファイルをダウンロードできません。 たとえば、PAC URL によって参照されているサーバーに到達できなかったか、サーバーから 404 NOT FOUND 応答が返された可能性があります";
		break;
	case ERROR_WINHTTP_UNHANDLED_SCRIPT_TYPE:
		psz = L"スクリプトの種類はサポートされていません。";
		break;
	case ERROR_WINHTTP_SCRIPT_EXECUTION_ERROR:
		psz = L"スクリプトの実行中にエラーが発生しました。";
		break;
 
	case ERROR_WINHTTP_NOT_INITIALIZED:
		psz = L"関数が初期化されていない状態で使用されました。";
		break;
	case ERROR_WINHTTP_SECURE_FAILURE:
		psz = L"サーバーから送信された Secure Sockets Layer (SSL) 証明書で、1 つ以上のエラーが見つかりました。";
		break;

	case ERROR_WINHTTP_SECURE_CERT_DATE_INVALID:
		psz = L"署名されたファイル内の現在のシステム クロックまたはタイムスタンプに対して検証するときに、必要な証明書が有効期間内にないことを示します。";
		break;
	case ERROR_WINHTTP_SECURE_CERT_CN_INVALID:
		psz = L"証明書 CN 名が渡された値と一致しません。";
		break;
	case ERROR_WINHTTP_SECURE_INVALID_CA:
		psz = L"証明書チェーンが処理されたが、信頼プロバイダーによって信頼されていないルート証明書で終了されました。";
		break;
	case ERROR_WINHTTP_SECURE_CERT_REV_FAILED:
		psz = L"失効サーバーがオフラインであったため、失効を確認出来ません。";
		break;
	case ERROR_WINHTTP_SECURE_CHANNEL_ERROR:
		psz = L"セキュリティで保護されたチャネルでエラーが発生した。";
		break;
	case ERROR_WINHTTP_SECURE_INVALID_CERT:
		psz = L"証明書が無効です。";
		break;
	case ERROR_WINHTTP_SECURE_CERT_REVOKED:
		psz = L"証明書が失効した。";
		break;
	case ERROR_WINHTTP_SECURE_CERT_WRONG_USAGE:
		psz = L"要求された使用に対して証明書が無効です。";
		break;

	case ERROR_WINHTTP_AUTODETECTION_FAILED:
		psz = L"プロキシ自動構成(PAC)ファイルのURLを検出出来ません。";
		break;
	case ERROR_WINHTTP_HEADER_COUNT_EXCEEDED:
		psz = L"受信できるヘッダー数よりも多くのヘッダーが応答に存在します。";
		break;
	case ERROR_WINHTTP_HEADER_SIZE_OVERFLOW:
		psz = L"受信したヘッダーのサイズが要求ハンドルの制限を超えました。";
		break;
	case ERROR_WINHTTP_CHUNKED_ENCODING_HEADER_SIZE_OVERFLOW:
		psz = L"チャンク エンコードの解析中にオーバーフロー条件が発生しました。";
		break;
	case ERROR_WINHTTP_RESPONSE_DRAIN_OVERFLOW:
		psz = L"受信応答が内部サイズ制限を超えました。";
		break;
	case ERROR_WINHTTP_CLIENT_CERT_NO_PRIVATE_KEY:
		psz = L"SSLクライアント証明書のコンテキストには、秘密キーが関連付けられません。";
		break;
	case ERROR_WINHTTP_CLIENT_CERT_NO_ACCESS_PRIVATE_KEY:
		psz = L"アプリケーションには、クライアント証明書に関連付けられている秘密キーにアクセスするために必要な特権がありません。";
		break;
 
	case ERROR_WINHTTP_CLIENT_AUTH_CERT_NEEDED_PROXY:
		psz = L"プロキシサーバーがセキュリティのためにクライアント認証を要求しています。";
		break;
	case ERROR_WINHTTP_SECURE_FAILURE_PROXY:
		psz = L"プロキシサーバーとセキュアなSSL/TLS接続を確立する過程で、証明書の検証失敗、暗号化アルゴリズムの不一致、その他のセキュリティ要件の不足などが発生してます。";
		break;
	case ERROR_WINHTTP_RESERVED_189:
		psz = L"未定義の予約されているエラーコード";
		break;
	case ERROR_WINHTTP_HTTP_PROTOCOL_MISMATCH:
		psz = L"HTTP プロトコルのバージョンが互いに一致していません。";
		break;
	case ERROR_WINHTTP_GLOBAL_CALLBACK_FAILED:
		psz = L"グローバル・コールバック関数の実行に失敗しました。";
		break;
	case ERROR_WINHTTP_FEATURE_DISABLED:
		psz = L"特定の機能が無効にされています。";
		break;
	}

	return psz;
}
