#include "Shared/OctoDenAssetNaming.h"

#include "Misc/PackageName.h"

namespace
{
	bool IsAsciiAlpha(const TCHAR Character)
	{
		return (Character >= TEXT('A') && Character <= TEXT('Z')) || (Character >= TEXT('a') && Character <= TEXT('z'));
	}

	bool IsAsciiDigit(const TCHAR Character)
	{
		return Character >= TEXT('0') && Character <= TEXT('9');
	}
}

FString OctoDenAssetNaming::SanitizeAssetName(const FString& RawName, const FString& FallbackStem)
{
	FString Result;
	Result.Reserve(RawName.Len());

	for (const TCHAR Character : RawName)
	{
		if (FChar::IsAlnum(Character) || Character == TEXT('_'))
		{
			Result.AppendChar(Character);
		}
	}

	if (Result.IsEmpty())
	{
		Result = FallbackStem;
	}

	if (Result.IsEmpty())
	{
		Result = TEXT("GeneratedAsset");
	}

	if (Result[0] >= TEXT('0') && Result[0] <= TEXT('9'))
	{
		Result = FString(TEXT("A")) + Result;
	}

	return Result;
}

FString OctoDenAssetNaming::NormalizePackageFolder(const FString& RawFolder, const FString& FallbackFolder)
{
	FString Result = RawFolder.TrimStartAndEnd();
	if (Result.IsEmpty())
	{
		Result = FallbackFolder;
	}

	Result.ReplaceInline(TEXT("\\"), TEXT("/"));
	if (!Result.StartsWith(TEXT("/")))
	{
		Result = FString(TEXT("/")) + Result;
	}
	if (Result.EndsWith(TEXT("/")))
	{
		Result.LeftChopInline(1);
	}

	if (!Result.StartsWith(TEXT("/Game")))
	{
		Result = FallbackFolder;
	}

	if (!FPackageName::IsValidLongPackageName(Result))
	{
		return FallbackFolder;
	}

	return Result;
}

FString OctoDenAssetNaming::BuildPackagePath(const FString& FolderPath, const FString& AssetName)
{
	const FString SafeFolder = NormalizePackageFolder(FolderPath, TEXT("/Game"));
	const FString SafeAssetName = SanitizeAssetName(AssetName, TEXT("GeneratedAsset"));
	return FString::Printf(TEXT("%s/%s"), *SafeFolder, *SafeAssetName);
}

FString OctoDenAssetNaming::BuildObjectPath(const FString& FolderPath, const FString& AssetName)
{
	const FString SafeFolder = NormalizePackageFolder(FolderPath, TEXT("/Game"));
	const FString SafeAssetName = SanitizeAssetName(AssetName, TEXT("GeneratedAsset"));
	return FString::Printf(TEXT("%s/%s.%s"), *SafeFolder, *SafeAssetName, *SafeAssetName);
}

FString OctoDenAssetNaming::ToPascalIdentifier(const FString& RawName, const FString& FallbackStem, const TCHAR* RequiredPrefix)
{
	TArray<FString> Tokens;
	RawName.ParseIntoArray(Tokens, TEXT(" _-/\\."), true);

	FString Result;
	for (FString Token : Tokens)
	{
		Token = SanitizeAssetName(Token, FString());
		if (Token.IsEmpty())
		{
			continue;
		}

		Token[0] = FChar::ToUpper(Token[0]);
		Result += Token;
	}

	if (Result.IsEmpty())
	{
		Result = SanitizeAssetName(FallbackStem, TEXT("GeneratedClass"));
	}

	if (!Result.IsEmpty() && IsAsciiDigit(Result[0]))
	{
		Result = FString(TEXT("N")) + Result;
	}

	if (RequiredPrefix != nullptr && FCString::Strlen(RequiredPrefix) > 0 && !Result.StartsWith(RequiredPrefix))
	{
		Result = FString(RequiredPrefix) + Result;
	}

	return Result;
}
